#!/usr/bin/bash
set -e

PROGRAM=$0
COMMAND=$1
NAME=$(basename "$PROGRAM")
CMAKE_ARGS=()

print_help() {
    cat <<EOF
Usage: $NAME [command] [architecture] [toolchain] [args...]
  Supported architectures: i686
    Default is NASHIOS_ARCH environment variable, or i686 if not defined

  Supported toolchains: gnu
    Defaults is NASHIOS_TOOLCHAIN, or gnu if not defined

  Supported commands:
    build:              Compiles architecture binaries, [args...] are passed to cmake
    install:            Install the architecture binary
    image:              Creates a disk image with the installed binaries
    run:                Run the built image in the virtualizer
    delete:             Removes the architecture build environment
    recreate:           Remove and recreate the build environment for the architecture
    rebuild:            Removes and recreates the built environment and builds to architecture
    rebuild-toolchain:  Remove and rebuild the architecture toolchain
    rebuild-world:      Remove and rebuild the toolchain and create the environment for architecture
EOF
}

usage() {
    >&2 print_help
    exit 1
}

[ -n "$COMMAND" ] || usage
shift

if [ "$COMMAND" = "help" ]; then
    print_help
    exit 0
fi

if [ -n "$1" ]; then
    ARCHITECTURE="$1"; shift
else
    ARCHITECTURE="${NASHIOS_ARCH:-"i686"}"
fi
export NASHIOS_ARCH=${ARCHITECTURE}

if [ -n "$1" ]; then
    TOOLCHAIN="$1"; shift
else
    TOOLCHAIN="${NASHIOS_TOOLCHAIN:-"gnu"}"
fi
if ! [[ "${TOOLCHAIN}" =~ ^(gnu)$ ]]; then
    >&2 echo "Unknown toolchain: ${TOOLCHAIN}"
    exit 1
fi

buildstep() {
    name=$1
    shift
    "$@" 2>&1 | sed $'s|^|\x1b[32m['"${name}"$']\x1b[39m |'
}
export -f buildstep

fail() {
    echo "FAIL: $*"
    exit 1
}
export -f fail

exit_if_root() {
    if [ "$(id -u)" -eq 0 ]; then
       fail "$*"
    fi
}
export -f exit_if_root

exit_if_root "It is forbidden to run the $NAME script as root"

compile_cmake() {
    echo "CMake version is not supported"
    ${SCRIPTS_DIR}/setup-cmake.sh
}

check_cmake() {
    if [ "$(cmake -P "${SOURCE_DIR}"/meta/cmake/cmake-version.cmake)" -ne 1 ]; then
        compile_cmake
    fi
}

compile_toolchain() {
    if [ ${TOOLCHAIN} = "gnu" ]; then
        ARCH=${ARCHITECTURE} ${SCRIPTS_DIR}/toolchain/setup-gnu.sh
    fi
}

check_toolchain() {
    [ -d "$CROSS_TOOLCHAIN_DIR" ] || compile_toolchain

    if [ ${TOOLCHAIN} = "gnu" ]; then
        CMAKE_ARGS+=("--toolchain ${SOURCE_DIR}/meta/cmake/toolchain/gnu-toolchain.cmake")
    fi
}

delete_toolchain() {
    [ ! -d "$CROSS_TOOLCHAIN_DIR" ] || rm -rf "$CROSS_TOOLCHAIN_DIR"
}

compile_architecture() {
    cmake -G Ninja "${CMAKE_ARGS[@]}" -B "$BUILD_ARCH_DIR"
}

check_architecture() {
    [ -d "$BUILD_ARCH_DIR/build.ninja" ] || compile_architecture
}

is_architecture_valid() {
    if [ "$ARCHITECTURE" = "i686" ]; then
        CMAKE_ARGS+=("-D PROCESSOR=x86")
        return 0
    fi
    return 1
}

delete_architecture() {
    [ ! -d "$BUILD_ARCH_DIR" ] || rm -rf "$BUILD_ARCH_DIR"
}

compile_target() {
    if [ $# -eq 0 ]; then
        cmake --build "$BUILD_ARCH_DIR"
    else
        ninja -j "$CORES" -C "$BUILD_ARCH_DIR" -- "$@"
    fi
}

project_root() {
    git rev-parse --show-toplevel
}

setup_variables() {
    is_architecture_valid || ( >&2 echo "Invalid architecture: $ARCHITECTURE"; usage )

    export SOURCE_DIR="$(project_root)"
    export SCRIPTS_DIR=${SOURCE_DIR}/meta/scripts

    export BINARY_DIR=${SOURCE_DIR}/binary
    export CACHE_DIR=${BINARY_DIR}/cache
    export BUILD_DIR=${BINARY_DIR}/build
    export CROSS_DIR=${BINARY_DIR}/cross
    export MOUNT_DIR=${BINARY_DIR}/mount

    export BUILD_COMMON_DIR=${BUILD_DIR}/common
    export BUILD_ARCH_DIR=${BUILD_DIR}/architecture/${ARCHITECTURE}/${TOOLCHAIN}
    export BUILD_TOOLCHAIN_DIR=${BUILD_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}

    export CROSS_COMMON_DIR=${CROSS_DIR}/common
    export CROSS_TOOLCHAIN_DIR=${CROSS_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}
    CMAKE_ARGS+=("-D TOOLCHAIN_PATH=${CROSS_TOOLCHAIN_DIR}")

    export SYSROOT_DIR=${BUILD_ARCH_DIR}/sysroot
    CMAKE_ARGS+=("-D SYSROOT_DIR=${SYSROOT_DIR}")

    export CORES=$(nproc)
    export PATH="${CROSS_COMMON_DIR}/bin":$PATH

    export TARGET=$ARCHITECTURE-pc-nashios
    CMAKE_ARGS+=("-D TOOLCHAIN_TARGET=${TARGET}")

    export DISK_IMAGE=${BUILD_ARCH_DIR}/disk.img
}

if [[ ${COMMAND} =~ ^(build|install|image|run|recreate|rebuild)$ ]]; then
    setup_variables
    check_cmake
    check_toolchain

    [[ "$COMMAND" != "recreate" && "$COMMAND" != "rebuild" ]] || delete_architecture
    check_architecture

    case "$COMMAND" in
        build)
            compile_target "$@"
            ;;
        install)
            compile_target
            compile_target install
            ;;
        image)
            compile_target
            compile_target install
            ${SCRIPTS_DIR}/setup-image.sh
            ;;
        run)
            compile_target
            compile_target install
            ${SCRIPTS_DIR}/setup-image.sh
            ${SCRIPTS_DIR}/run.sh
            ;;
        recreate)
            ;;
        rebuild)
            compile_target "$@"
            ;;
    esac
elif [[ ${COMMAND} = "delete" ]]; then
    setup_variables
    delete_architecture
elif [[ ${COMMAND} = "rebuild-toolchain" ]]; then
    setup_variables
    delete_toolchain
elif [[ ${COMMAND} = "rebuild-world" ]]; then
    setup_variables
    delete_architecture
    delete_toolchain
else
    >&2 echo "Unknown command: ${COMMAND}"
    usage
fi
