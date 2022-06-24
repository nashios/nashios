TOOLS_DIR=$(pwd)/tools
export MOUNT_DIR=$TOOLS_DIR/mount

fail() {
    echo "FAILED: $*"
    exit 1
}

IMAGE_ARCHIVE=qemu_disk.img
IMAGE_EXIST=0
if [ -f $IMAGE_ARCHIVE ]; then
    IMAGE_EXIST=1
    printf "Checking the existing image..."
    result=0
    e2fsck -f -y $IMAGE_ARCHIVE || result=$?
    if [ $result -ge 4 ]; then
        rm -f $IMAGE_ARCHIVE
        IMAGE_EXIST=0
        echo "Not using existing image"
    else
        echo "OK"
    fi
fi

disk_usage() {
    expr "$(du -sk --apparent-size "$1" | cut -f1)"
}

inode_usage() {
    find "$1" | wc -l
}

IMAGE_INODE_SIZE=128
IMAGE_INODE_COUNT=$((($(inode_usage sysroot) + 2000) * 7))
IMAGE_SIZE=$((($(disk_usage sysroot) * 1024) + (IMAGE_INODE_COUNT * IMAGE_INODE_SIZE) * 2))

if [ $IMAGE_EXIST -eq 1 ];  then
    OLD_IMAGE_SIZE=$(wc -c < $IMAGE_ARCHIVE)
    if [ "$IMAGE_SIZE" -gt "$OLD_IMAGE_SIZE" ]; then
        printf "Resizing image..."
        qemu-img resize -f raw $IMAGE_ARCHIVE "$IMAGE_SIZE" || fail "Resizing image"
        if ! resize2fs $IMAGE_ARCHIVE; then
            rm -f $IMAGE_ARCHIVE
            IMAGE_EXIST=0
            echo "Not using existing image"
        fi
        echo "OK"
    fi
fi

if [ $IMAGE_EXIST -ne 1 ]; then
    printf "Creating image..."
    qemu-img create -q -f raw $IMAGE_ARCHIVE "$IMAGE_SIZE" || fail "Creating image"
    chown "$SUDO_UID":"$SUDO_GID" $IMAGE_ARCHIVE || fail "Adjusting permissions on disk image"
    echo "OK"

    printf "Creating new filesystem..."
    mke2fs -q -I $IMAGE_INODE_SIZE -N $IMAGE_INODE_COUNT $IMAGE_ARCHIVE || fail "Creating filesystem"
    echo "OK"
fi

printf "Mounting filesystem..."
mkdir -p "$MOUNT_DIR"
if ! eval "fuse2fs $IMAGE_ARCHIVE $MOUNT_DIR/ -o fakeroot,rw"; then
    fail "Mounting filesystem"
else
    echo "OK"
fi

cleanup() {
    if [ -d "$MOUNT_DIR" ]; then
        printf "Unmounting filesystem..."
        fusermount -u "$MOUNT_DIR" || (sleep 1 && sync && fusermount -u "$MOUNT_DIR")
        echo "OK"
    fi
}
trap cleanup EXIT

"$SOURCE_DIR/scripts/setup-root-filesystem.sh"
