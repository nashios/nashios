CACHE_DIR=$TARGET_DIR/cache
BUILD_DIR=$TARGET_DIR/build
CROSS_DIR=$TARGET_DIR/cross

BINUTILS_VERSION=2.40
BINUTILS_PACKAGE=binutils-$BINUTILS_VERSION
BINUTILS_ARCHIVE=${BINUTILS_PACKAGE}.tar.xz
BINUTILS_MD5SUM="007b59bd908a737c06e5a8d3d2c737eb"

GCC_VERSION=13.1.0
GCC_PACKAGE=gcc-$GCC_VERSION
GCC_ARCHIVE=${GCC_PACKAGE}.tar.xz
GCC_MD5SUM="43e4de77f2218c83ca675257ea1af9ef"

CORES=$(nproc)
MIRROR=https://ftp.gnu.org/gnu

mkdir -p ${CACHE_DIR}
cd ${CACHE_DIR}

md5=""
if [ -e "${BINUTILS_ARCHIVE}" ]; then
    md5="$(md5sum $BINUTILS_ARCHIVE | cut -f1 -d' ')"
    echo "Binutils MD5='$md5'"
fi

if [ "$md5" != ${BINUTILS_MD5SUM} ] ; then
    rm -f ${BINUTILS_ARCHIVE}
    curl -LO "${MIRROR}/binutils/${BINUTILS_ARCHIVE}"
else
    echo "Skipped binutils download"
fi

if [ -e "$GCC_ARCHIVE" ]; then
    md5="$(md5sum $GCC_ARCHIVE | cut -f1 -d' ')"
    echo "Gcc MD5='$md5'"
fi

if [ "$md5" != ${GCC_MD5SUM} ] ; then
    rm -f ${GCC_ARCHIVE}
    curl -LO "${MIRROR}/gcc/${GCC_PACKAGE}/${GCC_ARCHIVE}"
else
    echo "Skipped gcc download"
fi

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

if [ ! -d "${BINUTILS_PACKAGE}" ]; then
    if [ -d ${BINUTILS_PACKAGE} ]; then
        rm -rf "${BINUTILS_PACKAGE}"
    fi
    echo "Extracting binutils"
    tar -xf ${CACHE_DIR}/${BINUTILS_ARCHIVE}
else
    echo "Using binutils from existing source directory"
fi

if [ ! -d "${GCC_PACKAGE}" ]; then
    if [ -d ${GCC_PACKAGE} ]; then
        rm -rf "${GCC_PACKAGE}"
    fi
    echo "Extracting gcc"
    tar -xf ${CACHE_DIR}/${GCC_ARCHIVE}
else
    echo "Using gcc from existing source directory"
fi

mkdir -p ${BUILD_DIR}/build-binutils
cd ${BUILD_DIR}/build-binutils
../${BINUTILS_PACKAGE}/configure --target=${TARGET} --prefix=${CROSS_DIR} --with-sysroot --disable-nls --disable-werror
make -j ${CORES}
make install

mkdir ${BUILD_DIR}/build-gcc
cd ${BUILD_DIR}/build-gcc
../${GCC_PACKAGE}/configure --target=${TARGET} --prefix=${CROSS_DIR} --disable-nls --enable-languages=c --without-headers
make -j ${CORES} all-gcc all-target-libgcc
make install-gcc install-target-libgcc