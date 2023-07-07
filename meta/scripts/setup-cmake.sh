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

if [ -z "${COMMAND}" ]; then
    COMMAND="all"
fi

all_command() {
    echo "CMake version: ${CMAKE_VERSION}"
    echo "CMake checksum: ${CMAKE_CHECKSUM}"

    mkdir -p "${CACHE_DIR}"
    pushd "${CACHE_DIR}"
    local md5=""
    if [ -e "${CMAKE_ARCHIVE}" ]; then
        md5="$(md5sum $CMAKE_ARCHIVE | cut -f1 -d' ')"
        buildstep "CMake/download" echo "MD5='$md5'"
    fi

    if [ "$md5" != "${CMAKE_CHECKSUM}" ]; then
        rm -f ${CMAKE_ARCHIVE}
        buildstep "CMake/download" wget "${MIRROR}/v${CMAKE_VERSION}/${CMAKE_ARCHIVE}"
    else
        buildstep "CMake/download" echo "Skipped download"
    fi
    popd

    mkdir -p "${COMMON_BUILD_DIR}"
    pushd "${COMMON_BUILD_DIR}"
    if [ ! -d "${CMAKE_PACKAGE}" ]; then
        buildstep "CMake/extract" echo "Extracting"
        buildstep "CMake/extract" tar -xf ${CACHE_DIR}/${CMAKE_ARCHIVE}
    else
        buildstep "CMake/extract" echo "Using from existing source directory"
    fi
    popd

    pushd "${COMMON_BUILD_DIR}/${CMAKE_PACKAGE}"
    buildstep "CMake/build" ./bootstrap --generator="${GENERATOR}" --parallel="${CORES}" --prefix="${COMMON_INSTALL_DIR}"
    buildstep "CMake/build" make -j "${CORES}" all
    buildstep "CMake/install" make install
    popd
}

help_command() {
    echo "Nashi Operating System Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>"
    echo "This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'."
    echo "This is free software, and you are welcome to redistribute it"
    echo "under certain conditions; type 'show c' for details."
    echo ""
    echo "Nashios CMake build script"
    echo ""
    echo "Usage: ${PROGRAM} [COMMAND] [OPTIONS...]"
    echo ""
    echo "Commands:"
    echo "  help                Display this help message"
    echo "  all                 Compile cmake"
    echo ""
    echo "Options:"
    echo "  mirror=<url>        Specify the mirror to download from (default: https://github.com/Kitware/CMake/releases/download)"
}

check_command() {
    if ! command -v cmake &>/dev/null; then
        echo "false"
        return
    fi

    if [[ "$(cmake --version | head -n1 | cut -d' ' -f3)" != "${CMAKE_VERSION}" ]]; then
        echo "false"
        return
    fi

    echo "true"
}

setup_variables() {
    CMAKE_VERSION="3.26.4"
    CMAKE_CHECKSUM="9095556a3b268fd88c995d2bb4c90320"
    CMAKE_PACKAGE="cmake-$CMAKE_VERSION"
    CMAKE_ARCHIVE="${CMAKE_PACKAGE}.tar.gz"

    MIRROR=$(find_in_args_or_default "mirror=" "https://github.com/Kitware/CMake/releases/download")
    GENERATOR="Unix Makefiles"
}

if [[ "${COMMAND}" = @(help|check|all) ]]; then
    setup_variables
    "${COMMAND}_command"
else
    echo "Unknown command: ${COMMAND}"
    echo "Type '${PROGRAM} help' for more information"
    exit 1
fi
