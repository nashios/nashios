TOOLS_DIR=$(pwd)/tools
CACHE_DIR=$TOOLS_DIR/cache
BUILD_DIR=$TOOLS_DIR/build
CROSS_DIR=$TOOLS_DIR/cross

BINUTILS_VERSION=2.38
BINUTILS_PACKAGE=binutils-$BINUTILS_VERSION
BINUTILS_ARCHIVE=$BINUTILS_PACKAGE.tar.xz

GCC_VERSION=12.1.0
GCC_PACKAGE=gcc-$GCC_VERSION
GCC_ARCHIVE=$GCC_PACKAGE.tar.xz

MIRROR=http://ftp.gnu.org/gnu
TARGET=i686-nashios
PREFIX=$CROSS_DIR
CORES=$(nproc)

if [ "$USE_CACHED" = "true" ] ; then
    CACHE_FILE=$CACHE_DIR/github_actions_toolchain.tar.xz
    if [ -r "$CACHE_FILE" ] ; then
        echo "Toolchain cache $CACHE_FILE exists"
        echo "Extracting cache"
        if tar -xf "$CACHE_FILE" -C $TOOLS_DIR ; then
            echo "Successfully extracted cache"
            exit 0
        else
            echo "Cache extraction failed, most likely the cache is broken and should be removed"
            rm -f "$CACHE_FILE"
        fi
    else
        echo "Cache file $CACHE_FILE does not exist"
    fi
    echo "Starting the toolchain building process"
fi

mkdir -p $CACHE_DIR
cd $CACHE_DIR
wget -nc $MIRROR/binutils/$BINUTILS_ARCHIVE
wget -nc $MIRROR/gcc/$GCC_PACKAGE/$GCC_ARCHIVE

mkdir -p $BUILD_DIR
if [ ! -d $BUILD_DIR/$BINUTILS_PACKAGE ];then
    cd $BUILD_DIR || exit
    tar -xf $CACHE_DIR/$BINUTILS_ARCHIVE
    
    cd $BUILD_DIR/$BINUTILS_PACKAGE
    patch -p1 < $SOURCE_DIR/scripts/patches/binutils.patch > /dev/null
fi

if [ ! -d $BUILD_DIR/$GCC_PACKAGE ];then
    cd $BUILD_DIR || exit
    tar -xf $CACHE_DIR/$GCC_ARCHIVE

    cd $BUILD_DIR/$GCC_PACKAGE
    patch -p1 < $SOURCE_DIR/scripts/patches/gcc.patch > /dev/null
fi

mkdir -p $SYSROOT_DIR/usr/include
cd $SYSROOT_DIR
rsync -a --include='*.h' --include='*/' --exclude='*' --inplace $SOURCE_DIR/libraries/c/ $SYSROOT_DIR/usr/include/
rsync -a --include='*.h' --include='*/' --exclude='*' --inplace $SOURCE_DIR/kernel $SYSROOT_DIR/usr/include/

mkdir -p $BUILD_DIR/build-binutils
cd $BUILD_DIR/build-binutils
../$BINUTILS_PACKAGE/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT_DIR --disable-nls --disable-werror
make -j $CORES
make install

cd $BUILD_DIR/$GCC_PACKAGE || exit
./contrib/download_prerequisites

mkdir -p $BUILD_DIR/build-gcc
cd $BUILD_DIR/build-gcc
../$GCC_PACKAGE/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot=$SYSROOT_DIR --disable-nls --enable-languages=c
make -j $CORES all-gcc all-target-libgcc
make install-gcc install-target-libgcc

if [ "$USE_CACHED" = "true" ] ; then
    cd $TOOLS_DIR
    echo "Generating cache tar file"
    tar -cf $CACHE_FILE cross/
fi
