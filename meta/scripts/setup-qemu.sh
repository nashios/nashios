QEMU_VERSION=${QEMU_MAJOR_VERSION}.${QEMU_MINOR_VERSION}.${QEMU_PATCH_VERSION}
QEMU_PACKAGE=qemu-$QEMU_VERSION
QEMU_ARCHIVE=${QEMU_PACKAGE}.tar.xz
QEMU_MD5SUM="f871d2602f5db527533db0f13aa79ab9"

MIRROR=https://download.qemu.org

mkdir -p ${CACHE_DIR}
pushd ${CACHE_DIR}
    md5=""
    if [ -e "${QEMU_ARCHIVE}" ]; then
        md5="$(md5sum $QEMU_ARCHIVE | cut -f1 -d' ')"
        buildstep "QEMU" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${QEMU_MD5SUM} ] ; then
        rm -f ${QEMU_ARCHIVE}
        buildstep "QEMU" curl -LO "${MIRROR}/${QEMU_ARCHIVE}"
    else
        buildstep "QEMU" echo "Skipped download"
    fi
popd

mkdir -p ${COMMON_BUILD_DIR}
pushd ${COMMON_BUILD_DIR}
    if [ ! -d "${QEMU_PACKAGE}" ]; then
        buildstep "QEMU" echo "Extracting"
        buildstep "QEMU" tar -xf ${CACHE_DIR}/${QEMU_ARCHIVE}
    else
        buildstep "QEMU" echo "Using from existing source directory"
    fi
popd

pushd ${COMMON_BUILD_DIR}/${QEMU_PACKAGE}
    buildstep "QEMU" ./configure --prefix=${COMMON_CROSS_DIR} --target-list=i386-softmmu --enable-gtk
    buildstep "CMake" make -j ${CORES} all
    buildstep "CMake" make install
popd
