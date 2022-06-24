printf "Copying base system..."
rsync -aH --inplace "$SYSROOT_DIR/" "$MOUNT_DIR/"
echo "OK"
