#!/bin/bash
set -e

PROGRAM=$0
COMMAND=$1

NAME="$(basename "${PROGRAM}")"

usage() {
    echo "Nashios qemu build script, version ${VERSION}"
    echo ""
    echo "Usage: ${NAME} <command> <options>"
    echo ""
    echo "Commands:"
    echo "  help          Display this help message"
    echo "  all           Compile qemu"
    echo ""
    echo "Options:"
    echo "  mirror        Specify the mirror to download from"
    echo "  cores         Specify the number of cores to use for compilation"
    echo ""
    echo "  version       Specify the qemu version"
    echo "  checksum      Specify the qemu checksum"
    echo ""
    echo "  prefix_dir    Specify the installation prefix"
    echo "  build_dir     Specify the build directory"
    echo "  cache_dir     Specify the cache directory"
}
trap usage ERR

if [[ "${COMMAND}" = "help" || -z "${COMMAND}" ]]; then
    usage
    exit 0
fi
shift

ARGS=("$@")
QEMU_VERSION=$(find_in_args_or_fail "version=")
QEMU_MD5SUM=$(find_in_args_or_fail "checksum=")
QEMU_PACKAGE=qemu-$QEMU_VERSION
QEMU_ARCHIVE=${QEMU_PACKAGE}.tar.xz

echo "QEMU version: ${QEMU_VERSION}"
echo "QEMU checksum: ${QEMU_MD5SUM}"

CACHE_DIR=$(find_in_args_or_fail "cache_dir=")
echo "Cache directory: ${CACHE_DIR}"

MIRROR=$(find_in_args_or_default "mirror=" "https://download.qemu.org")
echo "Mirror: ${MIRROR}"

mkdir -p ${CACHE_DIR}
pushd ${CACHE_DIR}
md5=""
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

BUILD_DIR=$(find_in_args_or_fail "build_dir=")
echo "Build directory: ${BUILD_DIR}"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
if [ ! -d "${QEMU_PACKAGE}" ]; then
    buildstep "QEMU" echo "Extracting"
    buildstep "QEMU" tar -xf ${CACHE_DIR}/${QEMU_ARCHIVE}
else
    buildstep "QEMU" echo "Using from existing source directory"
fi
popd

PREFIX=$(find_in_args_or_fail "prefix_dir=")
echo "Installation prefix: ${PREFIX}"

TARGETS=$(find_in_args_or_fail "targets=")
echo "Targets: ${TARGETS}"

pushd ${BUILD_DIR}/${QEMU_PACKAGE}
buildstep "QEMU" ./configure --prefix=${PREFIX} --target-list=${TARGETS} --enable-gtk
buildstep "CMake" make -j ${CORES} all
buildstep "CMake" make install
popd
