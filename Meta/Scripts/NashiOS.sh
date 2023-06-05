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

  Supported toolchains: GNU
    Defaults is NASHIOS_TOOLCHAIN, or GNU if not defined

  Supported commands:
    build:              Compiles architecture binaries, [args...] are passed to cmake
    install:            Install the architecture binary
    image:              Creates a disk image with the installed binaries
    run:                Run the built image in the virtualizer
    toolchain:          Build the architecture toolchain
    clean:              Clean the architecture build environment
    clean-all:          Clean the toolchain and the architecture build environment
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

if ! [[ "${ARCHITECTURE}" =~ ^(i686)$ ]]; then
    >&2 echo "Unknown architecture: ${ARCHITECTURE}"
    exit 1
fi
export NASHIOS_ARCH=${ARCHITECTURE}

if [ -n "$1" ]; then
    TOOLCHAIN="$1"; shift
else
    TOOLCHAIN="${NASHIOS_TOOLCHAIN:-"GNU"}"
fi

if ! [[ "${TOOLCHAIN}" =~ ^(GNU)$ ]]; then
    >&2 echo "Unknown toolchain: ${TOOLCHAIN}"
    exit 1
fi
export NASHIOS_TOOLCHAIN=${TOOLCHAIN}

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
    ${SCRIPTS_DIR}/Common/SetupCMake.sh
}

check_cmake() {
    if [ "$(cmake -P ${SOURCE_DIR}/Meta/CMake/CMakeVersion.cmake)" -ne 1 ]; then
        compile_cmake
    fi
}

compile_qemu() {
    echo "QEMU version is not supported"
    ${SCRIPTS_DIR}/Common/SetupQEMU.sh
}

check_qemu() {
    export QEMU_MAJOR_VERSION=8
    export QEMU_MINOR_VERSION=0
    export QEMU_PATCH_VERSION=0

    if ! command -v "$NASHIOS_QEMU_BIN" >/dev/null 2>&1 ; then
        compile_qemu
    fi

    major_version=$("$NASHIOS_QEMU_BIN" -version | head -n 1 | sed -E 's/QEMU emulator version ([1-9][0-9]*|0).*/\1/')
    minor_version=$("$NASHIOS_QEMU_BIN" -version | head -n 1 | sed -E 's/QEMU emulator version [0-9]+\.([1-9][0-9]*|0).*/\1/')
    patch_version=$("$NASHIOS_QEMU_BIN" -version | head -n 1 | sed -E 's/QEMU emulator version [0-9]+\.[0-9]+\.([1-9][0-9]*|0).*/\1/')
    if [ "$major_version" -lt "$QEMU_MAJOR_VERSION" ] || 
        { [ "$major_version" -eq "$QEMU_MAJOR_VERSION" ] && 
            [ "$minor_version" -lt "$QEMU_MINOR_VERSION" ] && [ "$patch_version" -lt "$QEMU_PATCH_VERSION" ]; }; then
        compile_qemu
    fi
}

compile_toolchain() {
    if [ ${TOOLCHAIN} = "GNU" ]; then
        ARCH=${ARCHITECTURE} ${SCRIPTS_DIR}/Toolchain/SetupGNU.sh
    fi
}

check_toolchain() {
    [ -d "$CROSS_TOOLCHAIN_DIR" ] || compile_toolchain

    if [ ${TOOLCHAIN} = "GNU" ]; then
        CMAKE_ARGS+=("--toolchain ${SOURCE_DIR}/Meta/CMake/Toolchain/GNUToolchain.cmake")
    fi
}

clean_toolchain() {
    [ ! -d "$CROSS_TOOLCHAIN_DIR" ] || rm -rf "$CROSS_TOOLCHAIN_DIR"
}

compile_architecture() {
    cmake -G Ninja "${CMAKE_ARGS[@]}" -B "$BUILD_ARCH_DIR"
}

check_architecture() {
    [ -d "$BUILD_ARCH_DIR/build.ninja" ] || compile_architecture
}

clean_architecture() {
    [ ! -d "$BUILD_ARCH_DIR" ] || rm -rf "$BUILD_ARCH_DIR"
}

compile_target() {
    if [ $# -eq 0 ]; then
        cmake --build "$BUILD_ARCH_DIR"
    else
        ninja -j "$CORES" -C "$BUILD_ARCH_DIR" -- "$@"
    fi
}

setup_cmake_args() {
    CMAKE_ARGS+=("-D TOOLCHAIN_PATH=${CROSS_TOOLCHAIN_DIR}")
    CMAKE_ARGS+=("-D TOOLCHAIN_TARGET=${TARGET}")
    CMAKE_ARGS+=("-D SYSROOT_DIR=${SYSROOT_DIR}")

    if [ "$ARCHITECTURE" = "i686" ]; then
        CMAKE_ARGS+=("-D PROCESSOR=x86")
        CMAKE_ARGS+=("-D ARCHITECTURE=i686")
        return 0
    fi
}

setup_variables() {
    # Project directories
    export SOURCE_DIR="$(git rev-parse --show-toplevel)"
    export SCRIPTS_DIR=${SOURCE_DIR}/Meta/Scripts

    # Binary directories
    export BINARY_DIR=${SOURCE_DIR}/Binary
    export CACHE_DIR=${BINARY_DIR}/Cache
    export BUILD_DIR=${BINARY_DIR}/Build
    export CROSS_DIR=${BINARY_DIR}/Cross
    export MOUNT_DIR=${BINARY_DIR}/Mount
    export BUILD_COMMON_DIR=${BUILD_DIR}/Common
    export BUILD_ARCH_DIR=${BUILD_DIR}/Architecture/${ARCHITECTURE}/${TOOLCHAIN}
    export BUILD_TOOLCHAIN_DIR=${BUILD_DIR}/Toolchain/${ARCHITECTURE}/${TOOLCHAIN}
    export CROSS_COMMON_DIR=${CROSS_DIR}/Common
    export CROSS_TOOLCHAIN_DIR=${CROSS_DIR}/Toolchain/${ARCHITECTURE}/${TOOLCHAIN}
    export SYSROOT_DIR=${BUILD_ARCH_DIR}/Sysroot

    # Helpers
    export CORES=$(nproc)
    export TARGET=$ARCHITECTURE-pc-nashios
    export DISK_IMAGE=${BUILD_ARCH_DIR}/disk.img

    if [ "$ARCHITECTURE" = "i686" ]; then
        export NASHIOS_QEMU_BIN="qemu-system-i386"
    fi

    # Change path to use the compiled binaries
    export PATH="${CROSS_COMMON_DIR}/bin":$PATH

    setup_cmake_args
}

if [[ ${COMMAND} =~ ^(build|install|image|run)$ ]]; then
    setup_variables
    check_cmake
    check_toolchain
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
            ${SCRIPTS_DIR}/SetupImage.sh
            ;;
        run)
            check_qemu
            compile_target
            compile_target install
            ${SCRIPTS_DIR}/SetupImage.sh
            ${SCRIPTS_DIR}/Run.sh
            ;;
    esac
elif [[ ${COMMAND} = "clean" ]]; then
    setup_variables
    clean_architecture
elif [[ ${COMMAND} = "toolchain" ]]; then
    setup_variables
    clean_toolchain
    check_cmake
    check_toolchain
elif [[ ${COMMAND} = "clean-all" ]]; then
    setup_variables
    clean_architecture
    clean_toolchain
    check_cmake
    check_toolchain
    check_architecture
else
    >&2 echo "Unknown command: ${COMMAND}"
    usage
fi
