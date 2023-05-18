[ -z "$NASHIOS_QEMU_BIN" ] && NASHIOS_QEMU_BIN="qemu-system-i386"
[ -z "$NASHIOS_QEMU_KERNEL" ] && NASHIOS_QEMU_KERNEL="kernel/kernel"
[ -z "$NASHIOS_QEMU_RAM" ] && NASHIOS_QEMU_RAM="128M"
[ -z "$NASHIOS_QEMU_CPU" ] && NASHIOS_QEMU_CPU="max"
[ -z "$NASHIOS_DISK_IMAGE" ] && NASHIOS_DISK_IMAGE="disk.img"
[ -z "$NASHIOS_QEMU_ARGS" ] && NASHIOS_QEMU_ARGS="
    -kernel $NASHIOS_QEMU_KERNEL
    -m $NASHIOS_QEMU_RAM
    -cpu $NASHIOS_QEMU_CPU
    -drive file=${NASHIOS_DISK_IMAGE},format=raw,index=0,media=disk
    -serial stdio"

[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && NASHIOS_QEMU_VIRT="-enable-kvm"

"$NASHIOS_QEMU_BIN" $NASHIOS_QEMU_ARGS $NASHIOS_QEMU_VIRT