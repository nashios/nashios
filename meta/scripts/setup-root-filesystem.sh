echo "Installing base system"
if ! command -v rsync >/dev/null; then
    fail "Please install rsync"
fi

rsync -aH --inplace --update ${SOURCE_DIR}/base/ ${MOUNT_DIR}
rsync -aH --inplace --update ${SYSROOT_DIR}/ ${MOUNT_DIR}
echo "OK"
