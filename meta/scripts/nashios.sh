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
COMMAND="${1}"

if [[ -z "${COMMAND}" ]]; then
    echo "No command specified"
    echo "Type '${PROGRAM} help' for more information"
    exit 0
fi
shift

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
exit_if_root "It is forbidden to run the ${PROGRAM} script as root"

ARGS=("${@}")
find_in_args() {
    local match=$1
    for arg in "${ARGS[@]}"; do
        if [[ "${arg}" = "--${match}="* ]]; then
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

find_subcommand_in_args() {
    for arg in "${ARGS[@]}"; do
        if [[ "${arg}" != --* ]]; then
            echo "${arg}"
            return
        fi
    done
}
export -f find_subcommand_in_args

run_command() {
    "${SCRIPTS_DIR}/run.sh" ${@}
}

check_toolchain() {
    if [[ "${TOOLCHAIN}" = "gnu" ]]; then
        local result=$("${SCRIPTS_DIR}/setup-gnu-toolchain.sh" check)
        if [[ "${result}" = "true" ]]; then
            return 0
        fi
    fi
    return 1
}

build_cmake_command() {
    ${SCRIPTS_DIR}/setup-cmake.sh all
}

build_qemu_command() {
    local targets=()
    if [[ "${ARCHITECTURE}" = "i686" ]]; then
        targets+=("i386-softmmu")
    elif [[ "${ARCHITECTURE}" = "x86_64" ]]; then
        targets+=("x86_64-softmmu")
    fi

    ${SCRIPTS_DIR}/setup-qemu.sh all --targets="${targets[@]}"
}

build_toolchain_command() {
    if [[ "${TOOLCHAIN}" = "gnu" ]]; then
        ${SCRIPTS_DIR}/setup-gnu-toolchain.sh "${@}"
    fi
}

check_cmake_version() {
    local result=$("${SCRIPTS_DIR}/setup-cmake.sh" check)
    if [[ "${result}" = "true" ]]; then
        return 0
    fi
    return 1
}

check_cmake_generation() {
    if [[ ! -d "${SYSTEM_BUILD_DIR}" ]]; then
        return 1
    fi
    if [[ ! -f "${SYSTEM_BUILD_DIR}/CMakeCache.txt" ]]; then
        return 1
    fi
    return 0
}

CMAKE_ARGS=()
generate_cmake() {
    mkdir -p "${SYSTEM_BUILD_DIR}"
    pushd "${SYSTEM_BUILD_DIR}"
    buildstep "CMake/generate" cmake -S "${SOURCE_DIR}" -B "${SYSTEM_BUILD_DIR}" "${CMAKE_ARGS[@]}"
    popd
}

build_system_command() {
    if ! check_toolchain; then
        echo "The ${TOOLCHAIN} toolchain for ${ARCHITECTURE} is not compiled"
        echo "Type '${PROGRAM} build toolchain' to compile it"
        exit 1
    fi

    if ! check_cmake_version; then
        echo "CMake is not compiled"
        echo "Type '${PROGRAM} build cmake' to compile it"
        exit 1
    fi

    if ! check_cmake_generation; then
        echo "CMake is not generated"
        generate_cmake
    fi

    args=()
    if [[ $# -ne 0 ]]; then
        args=("--target $@")
    fi
    cmake --build ${SYSTEM_BUILD_DIR} ${args[@]}
    cmake --install ${SYSTEM_BUILD_DIR} ${args[@]}
}

build_command() {
    local command=$(find_subcommand_in_args)
    if [[ "${command}" = "help" ]]; then
        echo "Usage: ${PROGRAM} build [TARGET] [OPTIONS...]"
        echo ""
        echo "Targets:"
        echo "  help               Display this help message"
        echo "  system             Compile the system (default)"
        echo "  toolchain          Compile the toolchain"
        echo "  cmake              Compile CMake"
        echo "  qemu               Compile QEMU"
        echo ""
        echo "Options:"
        echo "  arch=<ARCH>        Architecture to build for (default: i686)"
        echo "  toolchain=<TOOL>   Toolchain to use (default: gnu)"
        exit 0
    fi

    if [[ -z "${command}" ]]; then
        command="system"
    else
        shift
    fi

    if [[ "${command}" = @(toolchain|system|cmake|qemu) ]]; then
        "build_${command}_command" "${@}"
    else
        echo "Unknown target: ${command}"
        echo "Type '${PROGRAM} build help' for more information"
        exit 1
    fi
}

clean_toolchain_command() {
    rm -rf ${TOOLCHAIN_BUILD_DIR}
    rm -rf ${TOOLCHAIN_INSTALL_DIR}

    echo "Toolchain build directory cleaned"
}

clean_common_command() {
    rm -rf "${COMMON_BUILD_DIR}"
    rm -rf "${COMMON_INSTALL_DIR}"

    echo "Common build directory cleaned"
}

clean_system_command() {
    rm -rf "${SYSTEM_BUILD_DIR}"
    rm -rf "${SYSTEM_INSTALL_DIR}"

    echo "System build directory cleaned"
}

clean_all_command() {
    clean_system_command
    clean_toolchain_command
    clean_common_command
}

clean_command() {
    local command=$(find_subcommand_in_args)
    if [[ "${command}" = "help" ]]; then
        echo "Usage: ${PROGRAM} clean [TARGET]"
        echo ""
        echo "Targets:"
        echo "  help                Display this help message"
        echo "  system              Clean the system build directory (default)"
        echo "  toolchain           Clean the toolchain build directory"
        echo "  common              Clean the common build directory"
        echo "  all                 Clean all targets"
        exit 0
    fi

    if [[ -z "${command}" ]]; then
        command="system"
    fi

    if [[ "${command}" = @(system|toolchain|common|all) ]]; then
        "clean_${command}_command" "${@}"
    else
        echo "Unknown target: ${command}"
        echo "Type '${PROGRAM} clean help' for more information"
        exit 1
    fi
}

help_command() {
    echo "Nashi Operating System Copyright (C) 2023 Saullo Bretas Silva <saullo.silva55@outlook.com>"
    echo "This program comes with ABSOLUTELY NO WARRANTY; for details type 'show w'."
    echo "This is free software, and you are welcome to redistribute it"
    echo "under certain conditions; type 'show c' for details."
    echo ""
    echo "Nashios build script"
    echo ""
    echo "Usage: ${PROGRAM} [COMMAND]"
    echo ""
    echo "Commands:"
    echo "  help               Display this help message"
    echo "  build              Compile the system"
    echo "  run                Run the system"
    echo "  clean              Clean the build directory"
    echo ""
    echo "Architectures:"
    echo "  i686               32-bit x86"
    echo "  x86_64             64-bit x86"
    echo ""
    echo "Toolchains:"
    echo "  gnu                GNU Compiler Collection"
    echo ""
    echo "Type '${PROGRAM} [COMMAND] help' for more information"
}

setup_variables() {
    export ARCHITECTURE=$(find_in_args_or_default "arch" "i686")
    if [[ "${ARCHITECTURE}" != @(i686|x86_64) ]]; then
        echo "Unknown architecture: ${ARCHITECTURE}"
        echo "Type '${PROGRAM} help' for more information"
        exit 1
    fi

    TOOLCHAIN=$(find_in_args_or_default "toolchain" "gnu")
    if [[ "${TOOLCHAIN}" != @(gnu) ]]; then
        echo "Unknown toolchain: ${TOOLCHAIN}"
        echo "Type '${PROGRAM} help' for more information"
        exit 1
    fi

    export TARGET="${ARCHITECTURE}-pc-nashios"
    export SOURCE_DIR=$(git rev-parse --show-toplevel)

    BINARY_DIR=${SOURCE_DIR}/binary
    export CACHE_DIR=${BINARY_DIR}/cache
    BUILD_DIR=${BINARY_DIR}/build
    INSTALL_DIR=${BINARY_DIR}/install
    MOUNT_DIR=${BINARY_DIR}/mount

    META_DIR=${SOURCE_DIR}/meta
    SCRIPTS_DIR=${META_DIR}/scripts
    CMAKE_DIR=${META_DIR}/cmake
    export PATCHES_DIR=${META_DIR}/patches
    export CONFIGS_DIR=${META_DIR}/configs

    export COMMON_BUILD_DIR=${BUILD_DIR}/common
    export SYSTEM_BUILD_DIR=${BUILD_DIR}/system/${ARCHITECTURE}/${TOOLCHAIN}
    export TOOLCHAIN_BUILD_DIR=${BUILD_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}

    export COMMON_INSTALL_DIR=${INSTALL_DIR}/common
    export TOOLCHAIN_INSTALL_DIR=${INSTALL_DIR}/toolchain/${ARCHITECTURE}/${TOOLCHAIN}
    export SYSTEM_INSTALL_DIR=${INSTALL_DIR}/system/${ARCHITECTURE}/${TOOLCHAIN}

    if [[ ${TOOLCHAIN} = "gnu" ]]; then
        CMAKE_ARGS+=("-DCMAKE_TOOLCHAIN_FILE=${CMAKE_DIR}/gnu-toolchain.cmake")
    fi

    CMAKE_ARGS+=("-DPROCESSOR=${ARCHITECTURE}")
    CMAKE_ARGS+=("-DTOOLCHAIN=${TOOLCHAIN_INSTALL_DIR}/bin/${TARGET}")
    CMAKE_ARGS+=("-DSYSROOT=${SYSTEM_INSTALL_DIR}")

    export PATH="${COMMON_INSTALL_DIR}/bin:$PATH"
    export CORES=$(find_in_args_or_default "cores=" "$(nproc)")
}

if [[ "${COMMAND}" = @(help|build|run|clean) ]]; then
    setup_variables
    "${COMMAND}_command" "${@}"
else
    echo "Unknown command: ${COMMAND}"
    echo "Type '${PROGRAM} help' for more information"
    exit 1
fi
