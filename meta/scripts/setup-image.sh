export MOUNT_DIR=mount
DISK_IMAGE=disk.img

fail() {
    echo "FAIL: $*"
    exit 1
}

FUSE2FS_FOUND=0
if [ "$(id -u)" != 0 ]; then
    if [ -x fuse2fs ] && fuse2fs --help 2>&1 |grep fakeroot > /dev/null; then
        FUSE2FS_FOUND=1
    else
        set +e
        sudo -E -- sh -c "\"$0\" $* || exit 42"
        case $? in
            1)
                fail "Script needs root privileges"
                ;;
            42)
                exit 1
                ;;
            *)
                exit 0
                ;;
        esac
    fi
else
    : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

disk_usage() {
    expr "$(du -sk --apparent-size "$1" | cut -f1)"
}

inode_usage() {
    find "$1" | wc -l
}

INODE_SIZE=128
INODE_COUNT=$(($(inode_usage "$SOURCE_DIR/base") + $(inode_usage sysroot)))
INODE_COUNT=$((INODE_COUNT + 2000))
DISK_SIZE_BYTES=$((($(disk_usage "$SOURCE_DIR/base") + $(disk_usage sysroot)) * 1024))
DISK_SIZE_BYTES=$((DISK_SIZE_BYTES + (INODE_COUNT * INODE_SIZE)))

if [ -z "$NASHIOS_DISK_SIZE_BYTES" ]; then
    DISK_SIZE_BYTES=$((DISK_SIZE_BYTES * 2))
    INODE_COUNT=$((INODE_COUNT * 7))
else
    if [ "$DISK_SIZE_BYTES" -gt "$NASHIOS_DISK_SIZE_BYTES" ]; then
        fail "NASHIOS_DISK_SIZE_BYTES is set to $NASHIOS_DISK_SIZE_BYTES bytes, but required disk size is $DISK_SIZE_BYTES bytes"
    fi
    DISK_SIZE_BYTES="$NASHIOS_DISK_SIZE_BYTES"
fi

if [ -n "$NASHIOS_INODE_COUNT" ]; then
    if [ "$INODE_COUNT" -gt "$NASHIOS_INODE_COUNT" ]; then
        fail "NASHIOS_INODE_COUNT is set to $NASHIOS_INODE_COUNT inodes, but required inode count is $INODE_COUNT"
    fi
    INODE_COUNT="$NASHIOS_INODE_COUNT"
fi

IMAGE_EXIST=0
if [ -f $DISK_IMAGE ]; then
    IMAGE_EXIST=1

    echo "Checking existing image"
    result=0
    e2fsck -f -y $DISK_IMAGE || result=$?
    if [ $result -ge 4 ]; then
        rm -f $DISK_IMAGE
        IMAGE_EXIST=0
        echo "Failed using existing image"
    else
        echo "OK"
    fi
fi

if [ $IMAGE_EXIST -eq 1 ];  then
    OLD_DISK_SIZE_BYTES=$(wc -c < $DISK_IMAGE)
    if [ "$DISK_SIZE_BYTES" -gt "$OLD_DISK_SIZE_BYTES" ]; then
        echo "Resizing disk image"
        qemu-img resize -f raw $DISK_IMAGE "$DISK_SIZE_BYTES" || fail "Failed to resize disk image"
        if ! resize2fs $DISK_IMAGE; then
            rm -f $DISK_IMAGE
            IMAGE_EXIST=0
            echo "Failed using existing image"
        fi
        echo "OK"
    fi
fi

if [ $IMAGE_EXIST -ne 1 ]; then
    echo "Creating disk image"
    qemu-img create -q -f raw $DISK_IMAGE "$DISK_SIZE_BYTES" || fail "Failed to create disk image"
    chown "$SUDO_UID":"$SUDO_GID" $DISK_IMAGE || fail "Failed to adjust permissions on disk image"
    echo "OK"

    echo "Creating new filesystem"
    mkfs.ext2 -q -I "${INODE_SIZE}" -N "${INODE_COUNT}" $DISK_IMAGE || fail "Failed to create filesystem"
    echo "OK"
fi

echo "Mounting filesystem"
mkdir -p $MOUNT_DIR
if [ $FUSE2FS_FOUND -eq 1 ]; then
    mount_cmd="fuse2fs $DISK_IMAGE $MOUNT_DIR/ -o fakeroot,rw"
else
    mount_cmd="mount $DISK_IMAGE $MOUNT_DIR/"
fi

if ! eval "$mount_cmd"; then
    fail "Failed to mount filesystem"
else
    echo "OK"
fi

cleanup() {
    if [ -d $MOUNT_DIR ]; then
        echo "Unmounting filesystem"
        if [ $FUSE2FS_FOUND -eq 1 ]; then
            fusermount -u $MOUNT_DIR || (sleep 1 && sync && fusermount -u $MOUNT_DIR)
        else
            umount $MOUNT_DIR || ( sleep 1 && sync && umount $MOUNT_DIR )
        fi
        rmdir $MOUNT_DIR
        echo "OK"
    fi
}
trap cleanup EXIT

"$SOURCE_DIR/meta/scripts/setup-root-filesystem.sh"