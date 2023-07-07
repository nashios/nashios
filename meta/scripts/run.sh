# Nashi Operating System
# Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

#!/bin/bash
set -e

PROGRAM="${0}"
COMMAND="${1}"
ARGS=("$@")

if [[ -z "${COMMAND}" ]]; then
    COMMAND="qemu"
fi

disk_usage() {
    expr "$(du -sk --apparent-size "$1" | cut -f1)"
}

inode_usage() {
    find "$1" | wc -l
}

create_disk() {
    INODE_SIZE="128"
    INODE_COUNT=$(($(inode_usage "${SOURCE_DIR}/base") + $(inode_usage ${SYSTEM_INSTALL_DIR})))
    INODE_COUNT=$((INODE_COUNT + 2000))
    DISK_SIZE_BYTES=$((($(disk_usage "${SOURCE_DIR}/base") + $(disk_usage ${SYSTEM_INSTALL_DIR})) * 1024))
    DISK_SIZE_BYTES=$((DISK_SIZE_BYTES + (INODE_COUNT * INODE_SIZE)))
    DISK_SIZE_BYTES=$((DISK_SIZE_BYTES * 2))
    INODE_COUNT=$((INODE_COUNT * 7))

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

    MOUNT_DIR=$(mktemp -d)
    echo "Mounting filesystem to ${MOUNT_DIR}"

    if [ "${IMAGE_EXIST}" -ne 1 ]; then
        echo "Creating disk image"
        qemu-img create -q -f raw "${DISK_IMAGE}" "${DISK_SIZE_BYTES}" || fail "Failed to create disk image"
        echo "OK"

        echo "Creating new filesystem"
        mkfs.ext2 -q -I "${INODE_SIZE}" -N "${INODE_COUNT}" "${DISK_IMAGE}" || fail "Failed to create filesystem"
        echo "OK"
    fi

    mkdir -p "${MOUNT_DIR}"
    fuse2fs "${DISK_IMAGE}" "${MOUNT_DIR}/" -o fakeroot,rw || fail "Failed to mount filesystem"
    echo "OK"

    echo "Installing base system"
    if ! command -v rsync >/dev/null; then
        fail "Please install rsync"
    fi

    rsync -aH --inplace --update "${SOURCE_DIR}/base/" "${MOUNT_DIR}"
    rsync -aH --inplace --update "${SYSTEM_INSTALL_DIR}/" "${MOUNT_DIR}"
    echo "OK"

    echo "Creating basic filesystem structure"
    for folder in bin boot dev etc home media mnt opt proc root run srv sys tmp usr var; do
        mkdir -p "${MOUNT_DIR}/${folder}"
    done
    echo "OK"

    if [ -d "${MOUNT_DIR}" ]; then
        echo "Unmounting filesystem"
        fusermount -u "${MOUNT_DIR}" || (sleep 1 && sync && fusermount -u "${MOUNT_DIR}")
        rmdir "${MOUNT_DIR}"
        echo "OK"
    fi
}

create_grub_iso() {
    mkdir -p "${SYSTEM_INSTALL_DIR}/boot/grub" && cp "${CONFIGS_DIR}/grub.cfg" "${SYSTEM_INSTALL_DIR}/boot/grub/grub.cfg"

    local grub=$(command -v grub-mkrescue 2>/dev/null)
    if [[ -z "${grub}" ]]; then
        grub=$(command -v grub2-mkrescue 2>/dev/null)
    fi
    "${grub}" -o "${CDROM_IMAGE}" "${SYSTEM_INSTALL_DIR}"
}

qemu_command() {
    create_disk
    create_grub_iso

    local qemu_args=()
    qemu_args+=("-cdrom" "${CDROM_IMAGE}")
    qemu_args+=("-drive" "file=${DISK_IMAGE},format=raw")
    qemu_args+=("-serial" "stdio")
    qemu_args+=("-cpu" "max")
    qemu_args+=("-enable-kvm")
    "${QEMU_BIN}" "${qemu_args[@]}"
}

help_command() {
    echo "Nashi Operating System Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>"
    echo "This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'."
    echo "This is free software, and you are welcome to redistribute it"
    echo "under certain conditions; type 'show c' for details."
    echo ""
    echo "Nashios run script"
    echo ""
    echo "Usage: ${PROGRAM} [COMMAND] [OPTIONS...]"
    echo ""
    echo "Commands:"
    echo "  help            Display this help message"
    echo "  qemu            Run the system in QEMU (default)"
}

setup_variables() {
    if [[ "${ARCHITECTURE}" = "i686" ]]; then
        QEMU_BIN="qemu-system-i386"
    elif [[ "${ARCHITECTURE}" = "x86_64" ]]; then
        QEMU_BIN="qemu-system-x86_64"
    fi

    DISK_IMAGE="${SYSTEM_BUILD_DIR}/disk.img"
    CDROM_IMAGE="${SYSTEM_BUILD_DIR}/cdrom.img"
}

if [[ "${COMMAND}" = @(help|qemu) ]]; then
    setup_variables
    "${COMMAND}_command"
else
    echo "Unknown command: ${COMMAND}"
    echo "Type '${PROGRAM} help' for more information"
    exit 1
fi
