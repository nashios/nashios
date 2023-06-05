BINUTILS_VERSION=2.40
BINUTILS_PACKAGE=binutils-$BINUTILS_VERSION
BINUTILS_ARCHIVE=${BINUTILS_PACKAGE}.tar.xz
BINUTILS_MD5SUM="007b59bd908a737c06e5a8d3d2c737eb"

GCC_VERSION=13.1.0
GCC_PACKAGE=gcc-$GCC_VERSION
GCC_ARCHIVE=${GCC_PACKAGE}.tar.xz
GCC_MD5SUM="43e4de77f2218c83ca675257ea1af9ef"

MIRROR=https://ftp.gnu.org/gnu

mkdir -p ${CACHE_DIR}
pushd ${CACHE_DIR}
    if [ "${USE_CACHE}" = "true" ] ; then
        CACHE_ARCHIVE="${CACHE_DIR}/toolchain-${NASHIOS_ARCH}-${NASHIOS_TOOLCHAIN}.tar.gz"
        if [ -r "${CACHE_ARCHIVE}" ] ; then
            buildstep "Cache" echo "Cache file ${CACHE_ARCHIVE} exists"
            buildstep "Cache" echo "Extracting toolchain from cache"

            mkdir -p ${CROSS_TOOLCHAIN_DIR}
            pushd ${CROSS_TOOLCHAIN_DIR}
                if tar -xzf "${CACHE_ARCHIVE}" ; then
                    buildstep "Cache" echo "Cache unchanged"
                    exit 0
                else
                    buildstep "Cache" echo "Failed to extract cached toolchain archive"
                    buildstep "Cache" echo "This means the cache is broken and will be removed"
                    buildstep "Cache" echo "Github Actions cannot update a cache, this will unnecessarily"
                    buildstep "Cache" echo "slow down all future builds for this hash, until the cache is resetted"
                    rm -f "${CACHE_ARCHIVE}"
                fi
            popd
        else
            buildstep "Cache" echo "Cache file ${CACHE_ARCHIVE} does not exist"
        fi
    fi

    md5=""
    if [ -e "${BINUTILS_ARCHIVE}" ]; then
        md5="$(md5sum $BINUTILS_ARCHIVE | cut -f1 -d' ')"
        buildstep "Binutils" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${BINUTILS_MD5SUM} ] ; then
        rm -f ${BINUTILS_ARCHIVE}
        buildstep "Binutils" curl -LO "${MIRROR}/binutils/${BINUTILS_ARCHIVE}"
    else
        buildstep "Binutils" echo "Skipped download"
    fi

    if [ -e "$GCC_ARCHIVE" ]; then
        md5="$(md5sum $GCC_ARCHIVE | cut -f1 -d' ')"
        buildstep "GCC" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${GCC_MD5SUM} ] ; then
        rm -f ${GCC_ARCHIVE}
        buildstep "GCC" curl -LO "${MIRROR}/gcc/${GCC_PACKAGE}/${GCC_ARCHIVE}"
    else
        buildstep "GCC" echo "Skipped download"
    fi
popd

mkdir -p ${BUILD_TOOLCHAIN_DIR}
pushd ${BUILD_TOOLCHAIN_DIR}
    if [ ! -d "${BINUTILS_PACKAGE}" ]; then
        buildstep "Binutils" echo "Extracting"
        buildstep "Binutils" tar -xf ${CACHE_DIR}/${BINUTILS_ARCHIVE}
        
        pushd ${BUILD_TOOLCHAIN_DIR}/${BINUTILS_PACKAGE}
            buildstep "Binutils" patch -p1 < "${SOURCE_DIR}/Meta/Patches/Binutils.patch" > /dev/null
        popd
    else
        buildstep "Binutils" echo "Using from existing source directory"
    fi

    if [ ! -d "${GCC_PACKAGE}" ]; then
        buildstep "GCC" echo "Extracting"
        buildstep "GCC" tar -xf ${CACHE_DIR}/${GCC_ARCHIVE}
        
        pushd ${BUILD_TOOLCHAIN_DIR}/${GCC_PACKAGE}
            buildstep "GCC" patch -p1 < "${SOURCE_DIR}/Meta/Patches/Gcc.patch" > /dev/null
        popd
    else
        buildstep "GCC" echo "Using from existing source directory"
    fi
popd

mkdir -p ${SYSROOT_DIR}/usr/include/kernel/api
buildstep "Headers" rsync -aH --include="*/" --include="*.h" --exclude="*" ${SOURCE_DIR}/Libraries/C/ ${SYSROOT_DIR}/usr/include
buildstep "Headers" rsync -aH --include="*/" --include="*.h" --exclude="*" ${SOURCE_DIR}/kernel/api/ ${SYSROOT_DIR}/usr/include/kernel/api

mkdir -p ${BUILD_TOOLCHAIN_DIR}/build-binutils
pushd ${BUILD_TOOLCHAIN_DIR}/build-binutils
    buildstep "Binutils/configure" ../${BINUTILS_PACKAGE}/configure --target=${TARGET} --prefix=${CROSS_TOOLCHAIN_DIR} --with-sysroot=${SYSROOT_DIR} --disable-nls
    buildstep "Binutils/compile" make -j ${CORES} all
    buildstep "Binutils/install" make install
popd

mkdir ${BUILD_TOOLCHAIN_DIR}/build-gcc
pushd ${BUILD_TOOLCHAIN_DIR}/build-gcc
    buildstep "GCC/configure" ../${GCC_PACKAGE}/configure --target=${TARGET} --prefix=${CROSS_TOOLCHAIN_DIR} --with-sysroot=${SYSROOT_DIR} --disable-nls --enable-languages=c,c++ --with-newlib
    buildstep "GCC/compile" make -j ${CORES} all-gcc all-target-libgcc
    buildstep "GCC/install" make install-gcc install-target-libgcc
    buildstep "GCC/libstdc++" make all-target-libstdc++-v3
    buildstep "GCC/libstdc++/install" make install-target-libstdc++-v3
popd

if [ "${USE_CACHE}" = "true" ] ; then
    pushd ${CROSS_TOOLCHAIN_DIR}
        buildstep "Cache" echo "Creating toolchain cache"
        buildstep "Cache" tar -czf "${CACHE_ARCHIVE}" .
    popd
fi
