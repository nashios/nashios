#!/bin/bash
set -e

PROGRAM=$0
COMMAND=$1

NAME="$(basename "${PROGRAM}")"
usage() {
    echo "Nashios run script, version ${VERSION}"
    echo ""
    echo "Usage: ${NAME} <command> <options>"
    echo ""
    echo "Commands:"
    echo "  help            Display this help message"
    echo "  qemu            Run the system in QEMU"
    echo ""
    echo "Options:"
    echo "  qemu_bin        Specify the QEMU binary"
    echo "  qemu_kernel     Specify the QEMU kernel"
    echo "  qemu_cpu        Specify the QEMU CPU"
    echo "  qemu_memory     Specify the QEMU memory"
    echo "  qemu_image      Specify the QEMU image"
    echo "  qemu_args       Specify additional QEMU arguments"
}
trap usage ERR

if [[ "${COMMAND}" = "help" || -z "${COMMAND}" ]]; then
    usage
    exit 0
fi
shift

ARGS=("$@")
qemu() {
    local qemu_bin=$(find_in_args_or_fail "qemu_bin=")
    local qemu_kernel=$(find_in_args_or_fail "qemu_kernel=")
    local qemu_image=$(find_in_args_or_fail "qemu_image=")
    local qemu_memory=$(find_in_args_or_default "qemu_memory=" "128M")
    local qemu_cpu=$(find_in_args_or_default "qemu_cpu=" "max")
    local qemu_args=$(find_in_args_or_default "qemu_args=" "")
    local qemu_accel=$(find_in_args_or_default "qemu_accel=" "true")

    qemu_args+=" -serial stdio"
    qemu_args+=" -drive file=${qemu_image},format=raw"

    if [[ "${qemu_accel}" = "true" ]]; then
        qemu_args+=" -enable-kvm"
    fi

    "${qemu_bin}" -kernel "${qemu_kernel}" -m "${qemu_memory}" -cpu "${qemu_cpu}" ${qemu_args}
}

if [[ "${COMMAND}" = @(qemu) ]]; then
    case "${COMMAND}" in
    qemu)
        qemu
        ;;
    esac
fi
