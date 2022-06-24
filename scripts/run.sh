[ -z "$NASHIOS_QEMU_BIN" ] && NASHIOS_QEMU_BIN="qemu-system-i386"
[ -z "$NASHIOS_QEMU_KERNEL" ] && NASHIOS_QEMU_KERNEL="kernel/kernel"
[ -z "$NASHIOS_QEMU_IMAGE" ] && NASHIOS_QEMU_IMAGE="qemu_disk.img"
[ -z "$NASHIOS_QEMU_ARGS" ] && NASHIOS_QEMU_ARGS="
    -kernel $NASHIOS_QEMU_KERNEL
    -serial stdio
    -drive file=$NASHIOS_QEMU_IMAGE,index=0,media=disk,format=raw"

[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && NASHIOS_QEMU_VIRT="-enable-kvm"

# shellcheck disable=SC2086
$NASHIOS_QEMU_BIN $NASHIOS_QEMU_ARGS $NASHIOS_QEMU_VIRT
