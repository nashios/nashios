PROGRAM=$0
COMMAND=$1

help() {
    echo "Usage: ${PROGRAM} [COMMAND] [ARCHITECTURE] [TOOLCHAIN]"
    echo "Architectures:"
    echo "  i686:               32-bit x86"
    echo ""
    echo "Toolchains:"
    echo "  gnu:                GNU Compiler Collection"
    echo ""
    echo "Commands:"
    echo "  help:               Display this help message"
    echo "  toolchain:          Compile the toolchain"
    echo "  build:              Compile the system"
    echo "  install:            Install all files to the root directory"
    echo "  image:              Create disk image"
    echo "  run:                Run the system in QEMU"
    echo "  clean:              Clean the system build directory"
    echo "  clean-common:       Clean the common build directory"
    echo "  clean-toolchain:    Clean the toolchain build directories"
    echo "  clean-all:          Clean the system, toolchain and common directories"
}

if [[ ${COMMAND} == "help" || -z ${COMMAND} ]]; then
    help
    exit 0
fi
shift

if [[ -z $1 ]]; then
    ARCHITECTURE="i686"
else
    if [[ $1 = @(i686) ]]; then
        ARCHITECTURE=$1
        shift
    else
        echo "Unknown architecture: $1"
        help
        exit 1
    fi
fi
export ARCHITECTURE

if [[ -z $1 ]]; then
    TOOLCHAIN="gnu"
else
    if [[ $1 = @(gnu) ]]; then
        TOOLCHAIN=$1
        shift
    else
        echo "Unknown toolchain: $1"
        help
        exit 1
    fi
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

CMAKE_ARGUMENTS=()
setup() {
    export SOURCE_DIR=$(git rev-parse --show-toplevel)

    # Binary directories
    BINARY_DIR=${SOURCE_DIR}/binary
    export CACHE_DIR=${BINARY_DIR}/cache
    BUILD_DIR=${BINARY_DIR}/build
    CROSS_DIR=${BINARY_DIR}/cross
    export MOUNT_DIR=${BINARY_DIR}/mount

    # Useful directories
    META_DIR=${SOURCE_DIR}/meta
    export SCRIPTS_DIR=${META_DIR}/scripts
    CMAKE_DIR=${META_DIR}/cmake
    export PATCHES_DIR=${META_DIR}/patches

    # Build directories
    export COMMON_BUILD_DIR=${BUILD_DIR}/common
    SYSTEM_BUILD_DIR=${BUILD_DIR}/system/${ARCHITECTURE}/${TOOLCHAIN}
    export SYSROOT_DIR=${SYSTEM_BUILD_DIR}/sysroot
    export TOOLCHAIN_BUILD_DIR=${BUILD_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}

    # Cross directories
    export COMMON_CROSS_DIR=${CROSS_DIR}/common
    export TOOLCHAIN_CROSS_DIR=${CROSS_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}

    # Helper variables
    export TARGET=${ARCHITECTURE}-pc-nashios
    export CORES=$(nproc)
    export DISK_IMAGE=${SYSTEM_BUILD_DIR}/disk.img

    # CMake
    if [[ ${TOOLCHAIN} = "gnu" ]]; then
        CMAKE_ARGUMENTS+=("-DCMAKE_TOOLCHAIN_FILE=${CMAKE_DIR}/gnu-toolchain.cmake")
    fi
    CMAKE_ARGUMENTS+=("-DPROCESSOR=${ARCHITECTURE}")
    CMAKE_ARGUMENTS+=("-DTOOLCHAIN=${TOOLCHAIN_CROSS_DIR}/bin/${TARGET}")
    CMAKE_ARGUMENTS+=("-DSYSROOT=${SYSROOT_DIR}")

    if [ "$ARCHITECTURE" = "i686" ]; then
        export NASHIOS_QEMU_BIN="qemu-system-i386"
    fi

    # Change path to use the compiled binaries
    export PATH="${COMMON_CROSS_DIR}/bin":$PATH
}

generate_cmake() {
    cmake -B ${SYSTEM_BUILD_DIR} -S ${SOURCE_DIR} ${CMAKE_ARGUMENTS[@]}
}

build() {
    args=()
    if [[ $# -ne 0 ]]; then
        args=("-t $@")
    fi
    cmake --build ${SYSTEM_BUILD_DIR} ${args[@]}
}

install() {
    args=()
    if [[ $# -ne 0 ]]; then
        args=("-t $@")
    fi
    cmake --install ${SYSTEM_BUILD_DIR} ${args[@]}
}

image() {
    build
    install
    ${SCRIPTS_DIR}/setup-image.sh
}

compile_qemu() {
    echo "QEMU version is not supported"
    ${SCRIPTS_DIR}/setup-qemu.sh
}

check_qemu() {
    export QEMU_MAJOR_VERSION=8
    export QEMU_MINOR_VERSION=0
    export QEMU_PATCH_VERSION=0

    if ! command -v "$NASHIOS_QEMU_BIN" >/dev/null 2>&1; then
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

run() {
    check_qemu
    image
    ${SCRIPTS_DIR}/run.sh
}

clean_toolchain() {
    rm -rf ${TOOLCHAIN_BUILD_DIR}
    rm -rf ${TOOLCHAIN_CROSS_DIR}

    echo "Removed all toolchain files"
}

compile_toolchain() {
    if [[ ${TOOLCHAIN} = "gnu" ]]; then
        ${SCRIPTS_DIR}/setup-gnu-toolchain.sh
    fi
}

toolchain() {
    setup

    if [[ -d ${TOOLCHAIN_CROSS_DIR} ]]; then
        read -p "The toolchain has already been compiled. Do you want to recompile it? [y/N] " yn
        case $yn in
        [Yy]*) clean_toolchain ;;
        *) exit 0 ;;
        esac
    fi

    compile_toolchain
}

clean() {
    rm -rf ${SYSTEM_BUILD_DIR}
    echo "Removed all system files"
}

clean_common() {
    rm -rf ${COMMON_BUILD_DIR}
    rm -rf ${COMMON_CROSS_DIR}
    echo "Removed all common files"
}

clean_all() {
    clean
    clean_toolchain

    echo "Removed all files"
}

compile_cmake() {
    echo "CMake version is not supported"
    ${SCRIPTS_DIR}/setup-cmake.sh
}

check_cmake() {
    if [ "$(cmake -P ${SOURCE_DIR}/meta/cmake/cmake-version.cmake)" -ne 1 ]; then
        compile_cmake
    fi
}

setup
if [[ ${COMMAND} = @(build|install|image|run) ]]; then
    check_cmake
    [[ -d ${TOOLCHAIN_CROSS_DIR} ]] || compile_toolchain
    [[ -d ${SYSTEM_BUILD_DIR}/Makefile ]] || generate_cmake

    case ${COMMAND} in
    build)
        build $@
        ;;
    install)
        build $@
        install $@
        ;;
    image)
        image
        ;;
    run)
        run
        ;;
    esac
elif [[ ${COMMAND} = "toolchain" ]]; then
    toolchain
elif [[ ${COMMAND} = "clean" ]]; then
    clean
elif [[ ${COMMAND} = "clean-common" ]]; then
    clean_common
elif [[ ${COMMAND} = "clean-toolchain" ]]; then
    clean_toolchain
elif [[ ${COMMAND} = "clean-all" ]]; then
    clean_all
else
    echo "Unknown command: ${COMMAND}"
    help
    exit 1
fi
