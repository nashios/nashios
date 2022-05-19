[ -z "$NASHIOS_QEMU_BIN" ] && NASHIOS_QEMU_BIN="qemu-system-i386"
[ -z "$NASHIOS_QEMU_KERNEL" ] && NASHIOS_QEMU_KERNEL="kernel/kernel"
[ -z "$NASHIOS_QEMU_ARGS" ] && NASHIOS_QEMU_ARGS="
    -kernel $NASHIOS_QEMU_KERNEL
    -serial stdio"

[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && NASHIOS_QEMU_VIRT="-enable-kvm"

$NASHIOS_QEMU_BIN $NASHIOS_QEMU_ARGS $NASHIOS_QEMU_VIRT
