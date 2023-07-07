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
    echo "No command specified"
    echo "Type '${PROGRAM} help' for more information"
    exit 0
fi
shift

all_command() {
    echo "QEMU version: ${QEMU_VERSION}"
    echo "QEMU checksum: ${QEMU_MD5SUM}"

    mkdir -p "${CACHE_DIR}"
    pushd "${CACHE_DIR}"
    local md5=""
    if [ -e "${QEMU_ARCHIVE}" ]; then
        md5="$(md5sum $QEMU_ARCHIVE | cut -f1 -d' ')"
        buildstep "QEMU" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${QEMU_MD5SUM} ]; then
        rm -f ${QEMU_ARCHIVE}
        buildstep "QEMU" wget "${MIRROR}/${QEMU_ARCHIVE}"
    else
        buildstep "QEMU" echo "Skipped download"
    fi
    popd

    mkdir -p "${COMMON_BUILD_DIR}"
    pushd "${COMMON_BUILD_DIR}"
    if [ ! -d "${QEMU_PACKAGE}" ]; then
        buildstep "QEMU" echo "Extracting"
        buildstep "QEMU" tar -xf ${CACHE_DIR}/${QEMU_ARCHIVE}
    else
        buildstep "QEMU" echo "Using from existing source directory"
    fi
    popd

    pushd "${COMMON_BUILD_DIR}/${QEMU_PACKAGE}"
    buildstep "QEMU" ./configure --prefix="${COMMON_INSTALL_DIR}" --target-list="${TARGETS}" --enable-gtk
    buildstep "CMake" make -j "${CORES}" all
    buildstep "CMake" make install
    popd
}

help_command() {
    echo "Nashi Operating System Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>"
    echo "This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'."
    echo "This is free software, and you are welcome to redistribute it"
    echo "under certain conditions; type 'show c' for details."
    echo ""
    echo "Nashios QEMU build script"
    echo ""
    echo "Usage: ${PROGRAM} [COMMAND] [OPTIONS...]"
    echo ""
    echo "Commands:"
    echo "  help                Display this help message"
    echo "  all                 Compile qemu"
    echo ""
    echo "Options:"
    echo "  mirror=<url>        Specify the mirror to download from (default: https://download.qemu.org)"
}

setup_variables() {
    QEMU_VERSION="8.0.2"
    QEMU_MD5SUM="15ee0d10f7a707ca22058da71c837a97"
    QEMU_PACKAGE="qemu-$QEMU_VERSION"
    QEMU_ARCHIVE="${QEMU_PACKAGE}.tar.xz"

    MIRROR=$(find_in_args_or_default "mirror" "https://download.qemu.org")
    TARGETS=$(find_in_args_or_fail "targets")

    if [[ "${ARCHITECTURE}" = "i686" ]]; then
        QEMU_BIN="qemu-system-i386"
    elif [[ "${ARCHITECTURE}" = "x86_64" ]]; then
        QEMU_BIN="qemu-system-x86_64"
    fi
}

check_command() {
    if ! command -v "${QEMU_BIN}" &>/dev/null; then
        echo "false"
        return
    fi

    if [[ "$("${QEMU_BIN}" --version | head -n1 | cut -d' ' -f4)" != "${QEMU_VERSION}" ]]; then
        echo "false"
        return
    fi

    echo "true"
}

if [[ "${COMMAND}" = @(help|all) ]]; then
    setup_variables
    "${COMMAND}_command"
else
    echo "Unknown command: ${COMMAND}"
    echo "Type '${PROGRAM} help' for more information"
    exit 1
fi
