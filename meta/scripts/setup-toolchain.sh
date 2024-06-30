BINUTILS_VERSION=2.42
BINUTILS_PACKAGE=binutils-${BINUTILS_VERSION}
BINUTILS_ARCHIVE=${BINUTILS_PACKAGE}.tar.xz

GCC_VERSION=14.1.0
GCC_PACKAGE=gcc-${GCC_VERSION}
GCC_ARCHIVE=${GCC_PACKAGE}.tar.xz

ARCH=x86
TARGET=i686-elf
MIRROR=https://ftp.gnu.org/gnu
CORES=$(nproc)

SOURCE_DIR=$(git rev-parse --show-toplevel)
CACHE_DIR=${SOURCE_DIR}/binary/cache
BUILD_DIR=${SOURCE_DIR}/binary/build/toolchain/${ARCH}
INSTALL_DIR=${SOURCE_DIR}/binary/install/toolchain/${ARCH}

mkdir -p ${CACHE_DIR}
cd ${CACHE_DIR}

wget -nc ${MIRROR}/binutils/${BINUTILS_ARCHIVE}
wget -nc ${MIRROR}/gcc/${GCC_PACKAGE}/${GCC_ARCHIVE}

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

tar -xf ${CACHE_DIR}/${BINUTILS_ARCHIVE}
tar -xf ${CACHE_DIR}/${GCC_ARCHIVE}

mkdir -p ${BUILD_DIR}/build-binutils
cd ${BUILD_DIR}/build-binutils
../${BINUTILS_PACKAGE}/configure --target=$TARGET --prefix="$INSTALL_DIR" --with-sysroot --disable-nls --disable-werror
make -j ${CORES} all
make install

mkdir -p ${BUILD_DIR}/build-gcc
cd ${BUILD_DIR}/build-gcc
../${GCC_PACKAGE}/configure --target=$TARGET --prefix="$INSTALL_DIR" --disable-nls --enable-languages=c --without-headers
make -j ${CORES} all-gcc all-target-libgcc
make install-gcc install-target-libgcc