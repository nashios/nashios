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
ARGS=("$@")

command=$(find_subcommand_in_args)
if [ -z "${command}" ]; then
    command="all"
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
    USE_CACHE=$(find_in_args_or_default "cache" "false")
    GENERATOR="Unix Makefiles"
}

if [[ "${command}" = @(help|check|all) ]]; then
    setup_variables

    if [ "${USE_CACHE}" = "true" ]; then
        mkdir -p ${CACHE_DIR}
        pushd ${CACHE_DIR}

        CACHE_ARCHIVE="${CACHE_DIR}/cmake.tar.gz"
        if [ -r "${CACHE_ARCHIVE}" ]; then
            buildstep "Cache" echo "Cache file ${CACHE_ARCHIVE} exists"
            buildstep "Cache" echo "Extracting cmake from cache"

            mkdir -p ${COMMON_INSTALL_DIR}
            pushd ${COMMON_INSTALL_DIR}
            if tar -xzf "${CACHE_ARCHIVE}"; then
                buildstep "Cache" echo "Cache unchanged"
                exit 0
            else
                buildstep "Cache" echo "Failed to extract cached cmake archive"
                buildstep "Cache" echo "This means the cache is broken and will be removed"
                buildstep "Cache" echo "Github Actions cannot update a cache, this will unnecessarily"
                buildstep "Cache" echo "slow down all future builds for this hash, until the cache is resetted"
                rm -f "${CACHE_ARCHIVE}"
            fi
            popd
        else
            buildstep "Cache" echo "Cache file ${CACHE_ARCHIVE} does not exist"
        fi
        popd
    fi

    "${command}_command"

    if [ "${USE_CACHE}" = "true" ]; then
        pushd ${COMMON_INSTALL_DIR}
        buildstep "Cache" echo "Creating toolchain cache"
        buildstep "Cache" tar -czf "${CACHE_ARCHIVE}" .
        popd
    fi
else
    echo "Unknown command: ${command}"
    echo "Type '${PROGRAM} help' for more information"
    exit 1
fi
