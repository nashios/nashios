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
    echo "  qemu_cdrom      Specify the QEMU CD-ROM"
    echo "  qemu_cpu        Specify the QEMU CPU"
    echo "  qemu_memory     Specify the QEMU memory"
    echo "  qemu_image      Specify the QEMU image"
    echo "  qemu_args       Specify additional QEMU arguments"
    echo "  qemu_mode       Specify the QEMU mode"
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
    local qemu_image=$(find_in_args_or_fail "qemu_image=")
    local qemu_kernel=$(find_in_args_or_default "qemu_kernel=" "")
    local qemu_cdrom=$(find_in_args_or_default "qemu_cdrom=" "")
    local qemu_memory=$(find_in_args_or_default "qemu_memory=" "128M")
    local qemu_cpu=$(find_in_args_or_default "qemu_cpu=" "max")
    local qemu_args=$(find_in_args_or_default "qemu_args=" "")
    local qemu_accel=$(find_in_args_or_default "qemu_accel=" "true")
    local qemu_mode=$(find_in_args_or_default "qemu_mode=" "kernel")

    qemu_args+=" -serial stdio"
    qemu_args+=" -drive file=${qemu_image},format=raw"
    qemu_args+=" -m ${qemu_memory}"
    qemu_args+=" -cpu ${qemu_cpu}"

    case "${qemu_mode}" in
    kernel)
        if [[ -z "${qemu_kernel}" ]]; then
            echo "qemu_kernel= is required for kernel mode"
            exit 1
        fi
        qemu_args+=" -kernel ${qemu_kernel}"
        ;;
    cdrom)
        if [[ -z "${qemu_cdrom}" ]]; then
            echo "qemu_cdrom= is required for cdrom mode"
            exit 1
        fi
        qemu_args+=" -cdrom ${qemu_cdrom}"
        ;;
    esac

    if [[ "${qemu_accel}" = "true" ]]; then
        qemu_args+=" -enable-kvm"
    fi

    "${qemu_bin}" ${qemu_args}
}

if [[ "${COMMAND}" = @(qemu) ]]; then
    case "${COMMAND}" in
    qemu)
        qemu
        ;;
    esac
fi
