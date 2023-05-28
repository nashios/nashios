#!/usr/bin/bash
set -e

COMMAND=$1

buildstep() {
    name=$1
    shift
    "$@" 2>&1 | sed $'s|^|\x1b[32m['"${name}"$']\x1b[39m |'
}
export -f buildstep

cmake_compile() {
    echo "CMake version is not supported"
    ${SCRIPTS_DIR}/setup-cmake.sh
}

check_binaries() {
    if [ "$(cmake -P "${SOURCE_DIR}"/meta/cmake/cmake-version.cmake)" -ne 1 ]; then
        cmake_compile
    fi
}

project_root() {
    git rev-parse --show-toplevel
}

setup_variables() {
    export SOURCE_DIR="$(project_root)"
    export SCRIPTS_DIR=${SOURCE_DIR}/meta/scripts
    export BINARY_DIR=${SOURCE_DIR}/binary
    export CACHE_DIR=${BINARY_DIR}/cache
    export BUILD_DIR=${BINARY_DIR}/build
    export CROSS_DIR=${BINARY_DIR}/cross
    mkdir -p ${CACHE_DIR} ${BUILD_DIR} ${CROSS_DIR}

    export CORES=$(nproc)
    export PATH="$CROSS_DIR/bin":$PATH
}

if [[ ${COMMAND} =~ ^()$ ]]; then
    setup_variables
    check_binaries
fi
