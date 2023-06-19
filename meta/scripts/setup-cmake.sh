#!/bin/bash
set -e

PROGRAM=$0
COMMAND=$1

NAME="$(basename "${PROGRAM}")"

usage() {
    echo "Nashios cmake build script, version ${VERSION}"
    echo ""
    echo "Usage: ${NAME} <command> <options>"
    echo ""
    echo "Commands:"
    echo "  help          Display this help message"
    echo "  all           Compile cmake"
    echo ""
    echo "Options:"
    echo "  mirror        Specify the mirror to download from"
    echo "  cores         Specify the number of cores to use for compilation"
    echo ""
    echo "  version       Specify the cmake version"
    echo "  checksum      Specify the cmake checksum"
    echo "  generator     Specify the cmake generator"
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
CMAKE_VERSION=$(find_in_args_or_fail "version=")
CMAKE_MD5SUM=$(find_in_args_or_fail "checksum=")
CMAKE_PACKAGE=cmake-$CMAKE_VERSION
CMAKE_ARCHIVE=${CMAKE_PACKAGE}.tar.gz

echo "CMake version: ${CMAKE_VERSION}"
echo "CMake checksum: ${CMAKE_MD5SUM}"

CACHE_DIR=$(find_in_args_or_fail "cache_dir=")
echo "Cache directory: ${CACHE_DIR}"

MIRROR=$(find_in_args_or_default "mirror=" "https://github.com/Kitware/CMake/releases/download")
echo "Mirror: ${MIRROR}"

mkdir -p ${CACHE_DIR}
pushd ${CACHE_DIR}
md5=""
if [ -e "${CMAKE_ARCHIVE}" ]; then
    md5="$(md5sum $CMAKE_ARCHIVE | cut -f1 -d' ')"
    buildstep "CMake/download" echo "MD5='$md5'"
fi

if [ "$md5" != ${CMAKE_MD5SUM} ]; then
    rm -f ${CMAKE_ARCHIVE}
    buildstep "CMake/download" curl -LO "${MIRROR}/v${CMAKE_VERSION}/${CMAKE_ARCHIVE}"
else
    buildstep "CMake/download" echo "Skipped download"
fi
popd

BUILD_DIR=$(find_in_args_or_fail "build_dir=")
echo "Build directory: ${BUILD_DIR}"

mkdir -p ${BUILD_DIR}
pushd ${BUILD_DIR}
if [ ! -d "${CMAKE_PACKAGE}" ]; then
    buildstep "CMake/extract" echo "Extracting"
    buildstep "CMake/extract" tar -xf ${CACHE_DIR}/${CMAKE_ARCHIVE}
else
    buildstep "CMake/extract" echo "Using from existing source directory"
fi
popd

PREFIX=$(find_in_args_or_fail "prefix_dir=")
echo "Installation prefix: ${PREFIX}"

GENERATOR=$(find_in_args_or_default "generator=" "Ninja")
echo "Generator: ${GENERATOR}"

CORES=$(find_in_args_or_default "cores=" "1")
echo "Cores: ${CORES}"

pushd ${BUILD_DIR}/${CMAKE_PACKAGE}
buildstep "CMake/build" ./bootstrap --generator=${GENERATOR} --parallel=${CORES} --prefix=${PREFIX}
buildstep "CMake/build" ninja -j ${CORES} all
buildstep "CMake/install" ninja install
popd
