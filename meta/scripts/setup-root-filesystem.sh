fail() {
    echo "FAIL: $*"
    exit 1
}

echo "Installing base system"
if ! command -v rsync >/dev/null; then
    fail "Please install rsync"
fi

if rsync --chown 2>&1 | grep "missing argument" >/dev/null; then
    rsync -aH --chown=0:0 --inplace --update ${SOURCE_DIR}/base/ ${MOUNT_DIR}/
    rsync -aH --chown=0:0 --inplace --update ${SYSROOT_DIR}/ ${MOUNT_DIR}/
else
    rsync -aH --inplace --update ${SOURCE_DIR}/base/ ${MOUNT_DIR}/
    rsync -aH --inplace --update ${SYSROOT_DIR}/ ${MOUNT_DIR}/
    chown -R 0:0 ${MOUNT_DIR}/
fi
echo "OK"
