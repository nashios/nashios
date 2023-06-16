echo "Installing base system"
if ! command -v rsync >/dev/null; then
    fail "Please install rsync"
fi

rsync -aH --inplace --update ${SOURCE_DIR}/base/ ${MOUNT_DIR}
rsync -aH --inplace --update ${SYSROOT_DIR}/ ${MOUNT_DIR}
echo "OK"

echo "Creating basic filesystem structure"
for folder in bin boot dev etc home media mnt opt proc root run srv sys tmp usr var; do
    mkdir -p ${MOUNT_DIR}/${folder}
done
echo "OK"
