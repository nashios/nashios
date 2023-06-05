disk_usage() {
    expr "$(du -sk --apparent-size "$1" | cut -f1)"
}

inode_usage() {
    find "$1" | wc -l
}

INODE_SIZE=128
INODE_COUNT=$(($(inode_usage "${SOURCE_DIR}/base") + $(inode_usage ${SYSROOT_DIR})))
INODE_COUNT=$((INODE_COUNT + 20000))
DISK_SIZE_BYTES=$((($(disk_usage "${SOURCE_DIR}/base") + $(disk_usage ${SYSROOT_DIR})) * 1024))
DISK_SIZE_BYTES=$((DISK_SIZE_BYTES + (INODE_COUNT * INODE_SIZE)))

if [ -z "$NASHIOS_DISK_SIZE_BYTES" ]; then
    DISK_SIZE_BYTES=$((DISK_SIZE_BYTES * 2))
    INODE_COUNT=$((INODE_COUNT))
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
mount $DISK_IMAGE $MOUNT_DIR || fail "Failed to mount filesystem"
echo "OK"

cleanup() {
    if [ -d $MOUNT_DIR ]; then
        echo "Unmounting filesystem"
        fusermount -u $MOUNT_DIR || (sleep 1 && sync && fusermount -u $MOUNT_DIR)
        rmdir $MOUNT_DIR
        echo "OK"
    fi
}
trap cleanup EXIT

"$SOURCE_DIR/meta/scripts/setup-root-filesystem.sh"
