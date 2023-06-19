#!/bin/bash
set -e

PROGRAM=$0
COMMAND=$1

NAME="$(basename "${PROGRAM}")"

usage() {
    echo "Nashios image build script, version ${VERSION}"
    echo ""
    echo "Usage: ${NAME} <command> <options>"
    echo ""
    echo "Commands:"
    echo "  help          Display this help message"
    echo "  all           Create the image"
    echo ""
    echo "Options:"
    echo "  disk_image    Specify the disk image"
    echo "  inode_size    Specify the inode size"
    echo "  inode_count   Specify the number of inodes"
    echo "  disk_size     Specify the disk size"
    echo ""
    echo "  source_dir    Specify the source directory"
    echo "  sysroot_dir   Specify the sysroot directory"
}
trap usage ERR

if [[ "${COMMAND}" = "help" || -z "${COMMAND}" ]]; then
    usage
    exit 0
fi
shift

ARGS=("$@")

INODE_SIZE=$(find_in_args_or_default "inode_size=" "128")
echo "Inode size: ${INODE_SIZE}"

INODE_COUNT=$(find_in_args_or_default "inode_count=" "")
DISK_SIZE=$(find_in_args_or_default "disk_size=" "")

SOURCE_DIR=$(find_in_args_or_fail "source_dir=")
echo "Source directory: ${SOURCE_DIR}"

SYSROOT_DIR=$(find_in_args_or_fail "sysroot_dir=")
echo "Sysroot directory: ${SYSROOT_DIR}"

if [ "${INODE_COUNT}" = "" ]; then
    disk_usage() {
        expr "$(du -sk --apparent-size "$1" | cut -f1)"
    }

    inode_usage() {
        find "$1" | wc -l
    }

    INODE_COUNT=$(($(inode_usage "${SOURCE_DIR}/base") + $(inode_usage ${SYSROOT_DIR})))
    INODE_COUNT=$((INODE_COUNT + 2000))
    DISK_SIZE_BYTES=$((($(disk_usage "${SOURCE_DIR}/base") + $(disk_usage ${SYSROOT_DIR})) * 1024))
    DISK_SIZE_BYTES=$((DISK_SIZE_BYTES + (INODE_COUNT * INODE_SIZE)))
    DISK_SIZE_BYTES=$((DISK_SIZE_BYTES * 2))
    INODE_COUNT=$((INODE_COUNT * 7))
fi

echo "Inode count: ${INODE_COUNT}"
echo "Disk size: ${DISK_SIZE_BYTES}"

DISK_IMAGE=$(find_in_args_or_fail "disk_image=")
echo "Disk image: ${DISK_IMAGE}"

IMAGE_EXIST=0
if [ -f "${DISK_IMAGE}" ]; then
    IMAGE_EXIST=1

    echo "Checking existing image"
    result=0
    e2fsck -f -y "${DISK_IMAGE}" || result=$?
    if [ $result -ge 4 ]; then
        rm -f "${DISK_IMAGE}"
        IMAGE_EXIST=0
        echo "Failed using existing image"
    else
        echo "OK"
    fi
fi

if [ "${IMAGE_EXIST}" -eq 1 ]; then
    old_size=$(wc -c <"${DISK_IMAGE}")
    if [ "${DISK_SIZE_BYTES}" -gt "${old_size}" ]; then
        echo "Resizing disk image"
        qemu-img resize -f raw "${DISK_IMAGE}" "${DISK_SIZE_BYTES}" || fail "Failed to resize disk image"
        if ! resize2fs "${DISK_IMAGE}"; then
            rm -f "${DISK_IMAGE}"
            IMAGE_EXIST=0
            echo "Failed using existing image"
        fi
        echo "OK"
    fi
fi

if [ "${IMAGE_EXIST}" -ne 1 ]; then
    echo "Creating disk image"
    qemu-img create -q -f raw "${DISK_IMAGE}" "${DISK_SIZE_BYTES}" || fail "Failed to create disk image"
    chown "$SUDO_UID":"$SUDO_GID" "${DISK_IMAGE}" || fail "Failed to adjust permissions on disk image"
    echo "OK"

    echo "Creating new filesystem"
    mkfs.ext2 -q -I "${INODE_SIZE}" -N "${INODE_COUNT}" "${DISK_IMAGE}" || fail "Failed to create filesystem"
    echo "OK"
fi

echo "Mounting filesystem"
export MOUNT_DIR=$(mktemp -d)
echo "Mounting filesystem to ${MOUNT_DIR}"

mkdir -p "${MOUNT_DIR}"
fuse2fs "${DISK_IMAGE}" "${MOUNT_DIR}/" -o fakeroot,rw || fail "Failed to mount filesystem"
echo "OK"

cleanup() {
    if [ -d "${MOUNT_DIR}" ]; then
        echo "Unmounting filesystem"
        fusermount -u "${MOUNT_DIR}" || (sleep 1 && sync && fusermount -u "${MOUNT_DIR}")
        rmdir "${MOUNT_DIR}"
        echo "OK"
    fi
}
trap cleanup EXIT

echo "Installing base system"
if ! command -v rsync >/dev/null; then
    fail "Please install rsync"
fi

rsync -aH --inplace --update "${SOURCE_DIR}/base/" "${MOUNT_DIR}"
rsync -aH --inplace --update "${SYSROOT_DIR}/" "${MOUNT_DIR}"
echo "OK"

echo "Creating basic filesystem structure"
for folder in bin boot dev etc home media mnt opt proc root run srv sys tmp usr var; do
    mkdir -p "${MOUNT_DIR}/${folder}"
done
echo "OK"
