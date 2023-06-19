#!/bin/bash
set -e

PROGRAM=$0
COMMAND=$1

NAME="$(basename "${PROGRAM}")"

usage() {
    echo "Nashios gnu toolchain build script, version ${VERSION}"
    echo ""
    echo "Usage: ${NAME} <command> <options>"
    echo ""
    echo "Commands:"
    echo "  help                        Display this help message"
    echo "  all                         Compile binutils and GCC"
    echo ""
    echo "Options:"
    echo "  cache=bool                  Enable caching"
    echo "  arch=arch                   Specify the architecture"
    echo "  target=triple               Specify the target triple"
    echo "  cores=number                Specify the number of cores to use for compilation"
    echo "  mirror=url                  Specify the mirror to download from"
    echo ""
    echo "  binutils_version=version    Specify the binutils version"
    echo "  binutils_checksum=checksum  Specify the binutils checksum"
    echo ""
    echo "  gcc_version=version         Specify the GCC version"
    echo "  gcc_checksum=checksum       Specify the GCC checksum"
    echo ""
    echo "  prefix_dir=path             Specify the installation prefix"
    echo "  cache_dir=path              Specify the cache directory"
    echo "  build_dir=path              Specify the build directory"
    echo "  source_dir=path             Specify the source directory"
    echo "  sysroot_dir=path            Specify the sysroot directory"
}
trap usage ERR

if [[ "${COMMAND}" = "help" || -z "${COMMAND}" ]]; then
    usage
    exit 0
fi
shift

ARGS=("$@")

BINUTILS_VERSION=$(find_in_args_or_fail "binutils_version=")
BINUTILS_MD5SUM=$(find_in_args_or_fail "binutils_checksum=")
BINUTILS_PACKAGE=binutils-$BINUTILS_VERSION
BINUTILS_ARCHIVE=${BINUTILS_PACKAGE}.tar.xz

echo "Binutils version: ${BINUTILS_VERSION}"
echo "Binutils checksum: ${BINUTILS_MD5SUM}"

GCC_VERSION=$(find_in_args_or_fail "gcc_version=")
GCC_MD5SUM=$(find_in_args_or_fail "gcc_checksum=")
GCC_PACKAGE=gcc-$GCC_VERSION
GCC_ARCHIVE=${GCC_PACKAGE}.tar.xz

echo "GCC version: ${GCC_VERSION}"
echo "GCC checksum: ${GCC_MD5SUM}"

MIRROR=$(find_in_args_or_default "mirror=" "https://ftp.gnu.org/gnu")
echo "Mirror: ${MIRROR}"

CACHE_DIR=$(find_in_args_or_fail "cache_dir=")
echo "Cache directory: ${CACHE_DIR}"

PREFIX=$(find_in_args_or_fail "prefix_dir=")
echo "Prefix: ${PREFIX}"

TARGET=$(find_in_args_or_fail "target=")
echo "Target: ${TARGET}"

USE_CACHE=$(find_in_args_or_default "cache=" "false")
echo "Use cache: ${USE_CACHE}"

mkdir -p ${CACHE_DIR}
pushd ${CACHE_DIR}
if [ "${USE_CACHE}" = "true" ]; then
    ARCH=$(echo ${TARGET} | cut -f1 -d-)
    echo "Architecture: ${ARCH}"

    CACHE_ARCHIVE="${CACHE_DIR}/toolchain-${ARCH}-gnu.tar.gz"
    if [ -r "${CACHE_ARCHIVE}" ]; then
        buildstep "Cache" echo "Cache file ${CACHE_ARCHIVE} exists"
        buildstep "Cache" echo "Extracting toolchain from cache"

        mkdir -p ${PREFIX}
        pushd ${PREFIX}
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
fi

md5=""
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

BUILD_DIR=$(find_in_args_or_fail "build_dir=")
echo "Build directory: ${BUILD_DIR}"

SOURCE_DIR=$(find_in_args_or_fail "source_dir=")
echo "Source directory: ${SOURCE_DIR}"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
if [ ! -d "${BINUTILS_PACKAGE}" ]; then
    buildstep "Binutils" echo "Extracting"
    buildstep "Binutils" tar -xf ${CACHE_DIR}/${BINUTILS_ARCHIVE}

    pushd ${BUILD_DIR}/${BINUTILS_PACKAGE}
    buildstep "Binutils" patch -p1 <"${SOURCE_DIR}/meta/patches/binutils.patch" >/dev/null
    popd
else
    buildstep "Binutils" echo "Using from existing source directory"
fi

if [ ! -d "${GCC_PACKAGE}" ]; then
    buildstep "GCC" echo "Extracting"
    buildstep "GCC" tar -xf ${CACHE_DIR}/${GCC_ARCHIVE}

    pushd ${BUILD_DIR}/${GCC_PACKAGE}
    buildstep "GCC" patch -p1 <"${SOURCE_DIR}/meta/patches/gcc.patch" >/dev/null
    popd
else
    buildstep "GCC" echo "Using from existing source directory"
fi
popd

SYSROOT_DIR=$(find_in_args_or_fail "sysroot_dir=")
echo "Sysroot directory: ${SYSROOT_DIR}"

buildstep "Headers" rsync -aH --mkpath --include="*/" --include="*.h" --exclude="*" ${SOURCE_DIR}/libraries/c/ ${SYSROOT_DIR}/usr/include
buildstep "Headers" rsync -aH --mkpath --include="*/" --include="*.h" --exclude="*" ${SOURCE_DIR}/kernel/api/ ${SYSROOT_DIR}/usr/include/kernel/api

CORES=$(find_in_args_or_default "cores=" "1")
echo "Cores: ${CORES}"

mkdir -p ${BUILD_DIR}/${BINUTILS_PACKAGE}-build
pushd ${BUILD_DIR}/${BINUTILS_PACKAGE}-build
buildstep "Binutils/configure" ../${BINUTILS_PACKAGE}/configure --target=${TARGET} --prefix=${PREFIX} --with-sysroot=${SYSROOT_DIR} --disable-werror
buildstep "Binutils/compile" make -j ${CORES} all
buildstep "Binutils/install" make install
popd

pushd ${BUILD_DIR}/${GCC_PACKAGE}
buildstep "GCC/contrib" ./contrib/download_prerequisites
popd

mkdir ${BUILD_DIR}/${GCC_PACKAGE}-build
pushd ${BUILD_DIR}/${GCC_PACKAGE}-build
buildstep "GCC/configure" ../${GCC_PACKAGE}/configure --target=${TARGET} --prefix=${PREFIX} --with-sysroot=${SYSROOT_DIR} --enable-languages=c,c++ --enable-shared --with-newlib
buildstep "GCC/compile" make -j ${CORES} all-gcc all-target-libgcc
buildstep "GCC/install" make install-gcc install-target-libgcc
buildstep "GCC/libstdc++" make all-target-libstdc++-v3
buildstep "GCC/libstdc++/install" make install-target-libstdc++-v3
popd

if [ "${USE_CACHE}" = "true" ]; then
    pushd ${PREFIX}
    buildstep "Cache" echo "Creating toolchain cache"
    buildstep "Cache" tar -czf "${CACHE_ARCHIVE}" .
    popd
fi
