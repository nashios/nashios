#!/bin/bash
set -e

PROGRAM=$0
COMMAND=$1

NAME="$(basename "${PROGRAM}")"
export VERSION=$(git describe --tags --always --dirty)

help() {
    echo "Nashios build script, version ${VERSION}"
    echo ""
    echo "Usage: ${NAME} <command> <architecture> <toolchain> <args...>"
    echo ""
    echo "Commands:"
    echo "  help:               Display this help message"
    echo "  toolchain:          Compile the toolchain"
    echo "  build:              Compile the system"
    echo "  install:            Install the system files"
    echo "  image:              Create disk image"
    echo "  run:                Run the system "
    echo "  clean:              Clean the system build directory"
    echo "  clean-common:       Clean the common build directory"
    echo "  clean-toolchain:    Clean the toolchain build directories"
    echo "  clean-all:          Clean the system, toolchain and common directories"
    echo ""
    echo "Architectures:"
    echo "  i686:               32-bit x86"
    echo "  x86_64:             64-bit x86"
    echo ""
    echo "Toolchains:"
    echo "  gnu:                GNU Compiler Collection"
}

if [[ "${COMMAND}" = "help" || -z "${COMMAND}" ]]; then
    help
    exit 0
fi
shift

if [[ -z "$1" ]]; then
    ARCHITECTURE="i686"
else
    if [[ "$1" = @(i686|x86_64) ]]; then
        ARCHITECTURE="$1"
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
exit_if_root "It is forbidden to run the ${NAME} script as root"

ARGS=("$@")
find_in_args() {
    local match=$1

    for arg in "${ARGS[@]}"; do
        if [[ "${arg}" = "${match}"* ]]; then
            echo "${arg#*=}"
            return
        fi
    done
}
export -f find_in_args

find_in_args_or_fail() {
    local match=$1
    local value=$(find_in_args "${match}")
    if [[ -z "${value}" ]]; then
        exit 1
    fi

    echo "${value}"
}
export -f find_in_args_or_fail

find_in_args_or_default() {
    local match=$1
    local default=$2
    local value=$(find_in_args "${match}")
    if [[ -z "${value}" ]]; then
        echo "${default}"
    else
        echo "${value}"
    fi
}
export -f find_in_args_or_default

setup_directories() {
    SOURCE_DIR=$(git rev-parse --show-toplevel)

    BINARY_DIR=${SOURCE_DIR}/binary
    CACHE_DIR=${BINARY_DIR}/cache
    BUILD_DIR=${BINARY_DIR}/build
    CROSS_DIR=${BINARY_DIR}/cross
    MOUNT_DIR=${BINARY_DIR}/mount

    META_DIR=${SOURCE_DIR}/meta
    SCRIPTS_DIR=${META_DIR}/scripts
    CMAKE_DIR=${META_DIR}/cmake
    PATCHES_DIR=${META_DIR}/patches
    CONFIGS_DIR=${META_DIR}/configs

    COMMON_BUILD_DIR=${BUILD_DIR}/common
    SYSTEM_BUILD_DIR=${BUILD_DIR}/system/${ARCHITECTURE}/${TOOLCHAIN}
    TOOLCHAIN_BUILD_DIR=${BUILD_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}

    COMMON_CROSS_DIR=${CROSS_DIR}/common
    TOOLCHAIN_CROSS_DIR=${CROSS_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}

    SYSROOT_DIR=${BINARY_DIR}/sysroot/${ARCHITECTURE}/${TOOLCHAIN}
}

CMAKE_ARGS=()
setup_cmake() {
    if [[ ${TOOLCHAIN} = "gnu" ]]; then
        CMAKE_ARGS+=("-DCMAKE_TOOLCHAIN_FILE=${CMAKE_DIR}/gnu-toolchain.cmake")
    fi

    CMAKE_ARGS+=("-DPROCESSOR=${ARCHITECTURE}")
    CMAKE_ARGS+=("-DTOOLCHAIN=${TOOLCHAIN_CROSS_DIR}/bin/${TARGET}")
    CMAKE_ARGS+=("-DSYSROOT=${SYSROOT_DIR}")
}

setup_version() {
    CMAKE_MAJOR_VERSION="3"
    CMAKE_MINOR_VERSION="26"
    CMAKE_PATCH_VERSION="4"
    CMAKE_VERSION="${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}"
    CMAKE_CHECKSUM="9095556a3b268fd88c995d2bb4c90320"

    QEMU_MAJOR_VERSION="8"
    QEMU_MINOR_VERSION="0"
    QEMU_PATCH_VERSION="2"
    QEMU_VERSION="${QEMU_MAJOR_VERSION}.${QEMU_MINOR_VERSION}.${QEMU_PATCH_VERSION}"
    QEMU_MD5SUM="15ee0d10f7a707ca22058da71c837a97"

    GCC_MAJOR_VERSION="13"
    GCC_MINOR_VERSION="1"
    GCC_PATCH_VERSION="0"
    GCC_VERSION="${GCC_MAJOR_VERSION}.${GCC_MINOR_VERSION}.${GCC_PATCH_VERSION}"
    GCC_MD5SUM="43e4de77f2218c83ca675257ea1af9ef"

    BINUTILS_MAJOR_VERSION="2"
    BINUTILS_MINOR_VERSION="40"
    BINUTILS_VERSION="${BINUTILS_MAJOR_VERSION}.${BINUTILS_MINOR_VERSION}"
    BINUTILS_MD5SUM="007b59bd908a737c06e5a8d3d2c737eb"
}

setup() {
    TARGET=${ARCHITECTURE}-pc-nashios

    setup_directories
    setup_cmake
    setup_version

    PATH="${COMMON_CROSS_DIR}/bin":$PATH
    CORES=$(find_in_args_or_default "cores=" "$(nproc)")

    if [[ "${ARCHITECTURE}" = "i686" ]]; then
        QEMU_BIN="qemu-system-i386"
    elif [[ "${ARCHITECTURE}" = "x86_64" ]]; then
        QEMU_BIN="qemu-system-x86_64"
    fi

    DISK_IMAGE="${SYSTEM_BUILD_DIR}/disk.img"
    CDROM_IMAGE="${SYSTEM_BUILD_DIR}/cdrom.iso"
}

generate_cmake() {
    cmake -B ${SYSTEM_BUILD_DIR} -S ${SOURCE_DIR} ${CMAKE_ARGS[@]}
}

check_cmake_generation() {
    if [[ ! -f "${SYSTEM_BUILD_DIR}/Makefile" ]]; then
        return 1
    fi
    return 0
}

check_cmake_version() {
    if ! command -v cmake &>/dev/null; then
        return 1
    fi

    if [[ "$(cmake --version | head -n1 | cut -d' ' -f3)" != "${CMAKE_VERSION}" ]]; then
        return 1
    fi

    return 0
}

build_cmake() {
    ${SCRIPTS_DIR}/SetupCMake.sh all \
        version="${CMAKE_VERSION}" \
        checksum="${CMAKE_CHECKSUM}" \
        prefix_dir="${COMMON_CROSS_DIR}" \
        build_dir="${COMMON_BUILD_DIR}" \
        cache_dir="${CACHE_DIR}" ||
        return 1

    return 0
}

build() {
    args=()
    if [[ $# -ne 0 ]]; then
        args=("--target $@")
    fi
    cmake --build ${SYSTEM_BUILD_DIR} ${args[@]}
}

install() {
    args=()
    if [[ $# -ne 0 ]]; then
        args=("--target $@")
    fi
    cmake --install ${SYSTEM_BUILD_DIR} ${args[@]}
}

image() {
    ${SCRIPTS_DIR}/setup-image.sh all \
        disk_image="${DISK_IMAGE}" \
        source_dir="${SOURCE_DIR}" \
        sysroot_dir="${SYSROOT_DIR}"
}

check_qemu_version() {
    if ! command -v "${QEMU_BIN}" &>/dev/null; then
        return 1
    fi

    if [[ "$(${QEMU_BIN} --version | head -n1 | cut -d' ' -f4)" != "${QEMU_VERSION}" ]]; then
        return 1
    fi

    return 0
}

build_qemu() {
    local targets=()
    if [[ "${ARCHITECTURE}" = "i686" ]]; then
        targets+=("i386-softmmu")
    elif [[ "${ARCHITECTURE}" = "x86_64" ]]; then
        targets+=("x86_64-softmmu")
    fi

    ${SCRIPTS_DIR}/setup-qemu.sh all \
        targets="${targets[@]}" \
        cores="${CORES}" \
        version="${QEMU_VERSION}" \
        checksum="${QEMU_MD5SUM}" \
        prefix_dir="${COMMON_CROSS_DIR}" \
        build_dir="${COMMON_BUILD_DIR}" \
        cache_dir="${CACHE_DIR}" ||
        return 1

    return 0
}

run() {
    build
    install
    image

    if ! check_qemu_version; then
        echo "QEMU is not compiled"
        if ! build_qemu; then
            echo "Failed to compile QEMU"
            exit 1
        fi
    fi

    local args=()
    args+=" qemu_bin=${QEMU_BIN}"
    args+=" qemu_image=${DISK_IMAGE}"

    local mode=$(find_in_args_or_default "run_mode=" "kernel")
    case "${mode}" in
    kernel)
        args+=" qemu_kernel=${SYSROOT_DIR}/boot/kernel"
        args+=" qemu_mode=kernel"
        ;;
    cdrom)
        mkdir -p "${SYSROOT_DIR}/boot/grub" && cp "${CONFIGS_DIR}/grub.cfg" "${SYSROOT_DIR}/boot/grub/grub.cfg"
        grub2-mkrescue -o "${CDROM_IMAGE}" "${SYSROOT_DIR}"

        args+=" qemu_cdrom=${CDROM_IMAGE}"
        args+=" qemu_mode=cdrom"
        ;;
    esac

    "${SCRIPTS_DIR}/run.sh" qemu ${args}
}

build_toolchain() {
    local use_cache=$(find_in_args_or_default "cache=" "false")

    if [[ "${TOOLCHAIN}" = "gnu" ]]; then
        if [[ "$(gcc -dumpversion)" < "${GCC_MAJOR_VERSION}" ]]; then
            echo "The system gcc is older than ${GCC_MAJOR_VERSION}, please install gcc ${GCC_MAJOR_VERSION} or newer"
            return 1
        fi

        ${SCRIPTS_DIR}/setup-gnu-toolchain.sh all \
            arch="${ARCHITECTURE}" \
            toolchain="${TOOLCHAIN}" \
            target="${TARGET}" \
            cores="${CORES}" \
            binutils_version="${BINUTILS_VERSION}" \
            binutils_checksum="${BINUTILS_MD5SUM}" \
            gcc_version="${GCC_VERSION}" \
            gcc_checksum="${GCC_MD5SUM}" \
            prefix_dir="${TOOLCHAIN_CROSS_DIR}" \
            build_dir="${TOOLCHAIN_BUILD_DIR}" \
            cache_dir="${CACHE_DIR}" \
            source_dir="${SOURCE_DIR}" \
            sysroot_dir="${SYSROOT_DIR}" \
            "${ARGS[@]}" ||
            return 1

        return 0
    fi

    return 1
}

check_toolchain() {
    if [[ "${TOOLCHAIN}" = "gnu" ]]; then
        if [[ ! -f "${TOOLCHAIN_CROSS_DIR}/bin/${TARGET}-gcc" ]]; then
            return 1
        fi
    fi
    return 0
}

clean_system() {
    rm -rf "${SYSTEM_BUILD_DIR}"

    echo "Cleaned the system build directory"
}

clean_toolchain() {
    if check_toolchain; then
        read -rp "The ${TOOLCHAIN} toolchain for ${ARCHITECTURE} is already compiled. Continue? [y/N] " input
        if [[ "${input}" != "y" ]]; then
            exit 0
        fi
    fi

    rm -rf "${TOOLCHAIN_BUILD_DIR}"
    rm -rf "${TOOLCHAIN_CROSS_DIR}"
    rm -rf "${CACHE_DIR}/toolchain-${ARCHITECTURE}-${TOOLCHAIN}"

    echo "Cleaned the toolchain build directories"
}

clean_common() {
    rm -rf "${COMMON_BUILD_DIR}"
    rm -rf "${COMMON_CROSS_DIR}"

    echo "Cleaned the common build directories"
}

clean_all() {
    clean_system
    clean_common
    clean_toolchain
}

check_before_build() {
    if ! check_toolchain; then
        echo "The ${TOOLCHAIN} toolchain for ${ARCHITECTURE} is not compiled"
        if ! build_toolchain; then
            echo "Failed to compile the toolchain"
            exit 1
        fi
    fi

    if ! check_cmake_version; then
        echo "CMake is not compiled"
        if ! build_cmake; then
            echo "Failed to compile CMake"
            exit 1
        fi
    fi

    if ! check_cmake_generation; then
        echo "CMake is not generated"
        generate_cmake
    fi
}

setup
if [[ "${COMMAND}" = @(build|install|image|run) ]]; then
    check_before_build

    case "${COMMAND}" in
    build)
        build $@
        ;;
    install)
        install $@
        ;;
    image)
        image
        ;;
    run)
        run
        ;;
    esac
elif [[ "${COMMAND}" = "toolchain" ]]; then
    clean_toolchain
    build_toolchain $@
elif [[ "${COMMAND}" = "clean" ]]; then
    clean_system
elif [[ "${COMMAND}" = "clean-common" ]]; then
    clean_common
elif [[ "${COMMAND}" = "clean-toolchain" ]]; then
    clean_toolchain
elif [[ "${COMMAND}" = "clean-all" ]]; then
    clean_all
else
    echo "Unknown command: ${COMMAND}"
    help
    exit 1
fi
