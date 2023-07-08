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
ARGS=("${@}")

command=$(find_subcommand_in_args)
if [ -z "${command}" ]; then
    command="all"
fi

binutils_is_installed() {
    if [ -f "${TOOLCHAIN_INSTALL_DIR}/bin/${TARGET}-ld" ]; then
        return 0
    fi
    return 1
}

binutils_command() {
    echo "Binutils version: ${BINUTILS_VERSION}"
    echo "Binutils checksum: ${BINUTILS_MD5SUM}"

    mkdir -p ${CACHE_DIR}
    pushd ${CACHE_DIR}
    local md5=""
    if [ -e "${BINUTILS_ARCHIVE}" ]; then
        md5="$(md5sum $BINUTILS_ARCHIVE | cut -f1 -d' ')"
        buildstep "Binutils" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${BINUTILS_MD5SUM} ]; then
        rm -f ${BINUTILS_ARCHIVE}
        buildstep "Binutils" wget "${MIRROR}/binutils/${BINUTILS_ARCHIVE}"
    else
        buildstep "Binutils" echo "Skipped download"
    fi
    popd

    mkdir -p ${TOOLCHAIN_BUILD_DIR}
    pushd ${TOOLCHAIN_BUILD_DIR}
    if [ ! -d "${BINUTILS_PACKAGE}" ]; then
        buildstep "Binutils" echo "Extracting"
        buildstep "Binutils" tar -xf ${CACHE_DIR}/${BINUTILS_ARCHIVE}

        pushd ${TOOLCHAIN_BUILD_DIR}/${BINUTILS_PACKAGE}
        buildstep "Binutils" patch -p1 <"${PATCHES_DIR}/binutils.patch" >/dev/null
        popd
    else
        buildstep "Binutils" echo "Using from existing source directory"
    fi
    popd

    mkdir -p ${TOOLCHAIN_BUILD_DIR}/${BINUTILS_PACKAGE}-build
    pushd ${TOOLCHAIN_BUILD_DIR}/${BINUTILS_PACKAGE}-build
    buildstep "Binutils/configure" ../${BINUTILS_PACKAGE}/configure --target=${TARGET} --prefix=${TOOLCHAIN_INSTALL_DIR} --with-sysroot=${SYSTEM_INSTALL_DIR} --disable-werror
    buildstep "Binutils/compile" make -j ${CORES} all
    buildstep "Binutils/install" make install
    popd
}

gcc_is_installed() {
    if [ -f "${TOOLCHAIN_INSTALL_DIR}/bin/${TARGET}-gcc" ]; then
        return 0
    fi
    return 1
}

gcc_command() {
    if ! binutils_is_installed; then
        echo "Binutils is not installed, cannot compile GCC"
        exit 1
    fi

    echo "GCC version: ${GCC_VERSION}"
    echo "GCC checksum: ${GCC_MD5SUM}"

    mkdir -p ${CACHE_DIR}
    pushd ${CACHE_DIR}
    local md5=""
    if [ -e "$GCC_ARCHIVE" ]; then
        md5="$(md5sum $GCC_ARCHIVE | cut -f1 -d' ')"
        buildstep "GCC" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${GCC_MD5SUM} ]; then
        rm -f ${GCC_ARCHIVE}
        buildstep "GCC" wget "${MIRROR}/gcc/${GCC_PACKAGE}/${GCC_ARCHIVE}"
    else
        buildstep "GCC" echo "Skipped download"
    fi
    popd

    mkdir -p ${TOOLCHAIN_BUILD_DIR}
    pushd ${TOOLCHAIN_BUILD_DIR}
    if [ ! -d "${GCC_PACKAGE}" ]; then
        buildstep "GCC" echo "Extracting"
        buildstep "GCC" tar -xf ${CACHE_DIR}/${GCC_ARCHIVE}

        pushd ${TOOLCHAIN_BUILD_DIR}/${GCC_PACKAGE}
        buildstep "GCC" patch -p1 <"${PATCHES_DIR}/gcc.patch" >/dev/null
        popd
    else
        buildstep "GCC" echo "Using from existing source directory"
    fi
    popd

    pushd ${TOOLCHAIN_BUILD_DIR}/${GCC_PACKAGE}
    buildstep "GCC/contrib" ./contrib/download_prerequisites
    popd

    mkdir -p ${TOOLCHAIN_BUILD_DIR}/${GCC_PACKAGE}-build
    pushd ${TOOLCHAIN_BUILD_DIR}/${GCC_PACKAGE}-build
    buildstep "GCC/configure" ../${GCC_PACKAGE}/configure --target=${TARGET} --prefix=${TOOLCHAIN_INSTALL_DIR} --with-sysroot=${SYSTEM_INSTALL_DIR} --enable-languages=c,c++ --enable-shared --with-newlib
    buildstep "GCC/compile" make -j ${CORES} all-gcc all-target-libgcc
    buildstep "GCC/install" make install-gcc install-target-libgcc
    popd
}

libstdc_command() {
    if ! gcc_is_installed; then
        echo "GCC is not installed, cannot compile libstdc++"
        exit 1
    fi

    pushd ${TOOLCHAIN_BUILD_DIR}/${GCC_PACKAGE}-build
    buildstep "GCC/libstdc++" make all-target-libstdc++-v3
    buildstep "GCC/libstdc++/install" make install-target-libstdc++-v3
    popd
}

help_command() {
    echo "Nashi Operating System Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>"
    echo "This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'."
    echo "This is free software, and you are welcome to redistribute it"
    echo "under certain conditions; type 'show c' for details."
    echo ""
    echo "Nashios Gnu Toolchain build script"
    echo ""
    echo "Usage: ${PROGRAM} [COMMAND] [OPTIONS...]"
    echo ""
    echo "Commands:"
    echo "  help                        Display this help message"
    echo "  all                         Compile binutils, gcc and libstdc++"
    echo "  binutils                    Compile binutils"
    echo "  gcc                         Compile gcc"
    echo "  libstdc                     Compile libstdc++"
    echo ""
    echo "Options:"
    echo "  cache=<bool>                Enable caching"
    echo "  arch=<ARCH>                 Specify the architecture (default: i686)"
    echo "  mirror=<url>                Specify the mirror to download from (default: https://ftp.gnu.org/gnu)"
}

all_command() {
    binutils_command
    gcc_command
    libstdc_command
}

check_command() {
    if ! binutils_is_installed; then
        echo "false"
        return
    fi

    if ! gcc_is_installed; then
        echo "false"
        return
    fi

    echo "true"
}

setup_variables() {
    BINUTILS_VERSION="2.40"
    BINUTILS_MD5SUM="007b59bd908a737c06e5a8d3d2c737eb"
    BINUTILS_PACKAGE="binutils-$BINUTILS_VERSION"
    BINUTILS_ARCHIVE="${BINUTILS_PACKAGE}.tar.xz"

    GCC_VERSION="13.1.0"
    GCC_MD5SUM="43e4de77f2218c83ca675257ea1af9ef"
    GCC_PACKAGE="gcc-$GCC_VERSION"
    GCC_ARCHIVE="${GCC_PACKAGE}.tar.xz"

    MIRROR=$(find_in_args_or_default "mirror=" "https://ftp.gnu.org/gnu")
    USE_CACHE=$(find_in_args_or_default "cache" "false")
}

if [[ "${command}" = @(help|all|binutils|gcc|libstdc|check) ]]; then
    setup_variables

    if [ "${USE_CACHE}" = "true" ]; then
        mkdir -p ${CACHE_DIR}
        pushd ${CACHE_DIR}

        CACHE_ARCHIVE="${CACHE_DIR}/toolchain-${ARCHITECTURE}-gnu.tar.gz"
        if [ -r "${CACHE_ARCHIVE}" ]; then
            buildstep "Cache" echo "Cache file ${CACHE_ARCHIVE} exists"
            buildstep "Cache" echo "Extracting toolchain from cache"

            mkdir -p ${TOOLCHAIN_INSTALL_DIR}
            pushd ${TOOLCHAIN_INSTALL_DIR}
            if tar -xzf "${CACHE_ARCHIVE}"; then
                buildstep "Cache" echo "Cache unchanged"
                exit 0
            else
                buildstep "Cache" echo "Failed to extract cached toolchain archive"
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

    buildstep "Headers" rsync -aH --mkpath --include="*/" --include="*.h" --exclude="*" ${SOURCE_DIR}/libraries/c/ ${SYSTEM_INSTALL_DIR}/usr/include
    buildstep "Headers" rsync -aH --mkpath --include="*/" --include="*.h" --exclude="*" ${SOURCE_DIR}/kernel/api/ ${SYSTEM_INSTALL_DIR}/usr/include/kernel/api

    "${command}_command" "${@}"

    if [ "${USE_CACHE}" = "true" ]; then
        pushd ${TOOLCHAIN_INSTALL_DIR}
        buildstep "Cache" echo "Creating toolchain cache"
        buildstep "Cache" tar -czf "${CACHE_ARCHIVE}" .
        popd
    fi
else
    echo "Unknown command: ${command}"
    echo "Type '${PROGRAM} help' for more information"
    exit 1
fi
