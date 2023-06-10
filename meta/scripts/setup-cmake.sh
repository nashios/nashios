CMAKE_VERSION=3.26.4
CMAKE_PACKAGE=cmake-$CMAKE_VERSION
CMAKE_ARCHIVE=${CMAKE_PACKAGE}.tar.gz
CMAKE_MD5SUM="9095556a3b268fd88c995d2bb4c90320"

GENERATOR=Ninja
MIRROR=https://github.com/Kitware/CMake/releases/download

mkdir -p ${CACHE_DIR}
pushd ${CACHE_DIR}
    md5=""
    if [ -e "${CMAKE_ARCHIVE}" ]; then
        md5="$(md5sum $CMAKE_ARCHIVE | cut -f1 -d' ')"
        buildstep "CMake" echo "MD5='$md5'"
    fi

    if [ "$md5" != ${CMAKE_MD5SUM} ] ; then
        rm -f ${CMAKE_ARCHIVE}
        buildstep "CMake" curl -LO "${MIRROR}/v${CMAKE_VERSION}/${CMAKE_ARCHIVE}"
    else
        buildstep "CMake" echo "Skipped download"
    fi
popd

mkdir -p ${COMMON_BUILD_DIR}
pushd ${COMMON_BUILD_DIR}
    if [ ! -d "${CMAKE_PACKAGE}" ]; then
        buildstep "CMake" echo "Extracting"
        buildstep "CMake" tar -xf ${CACHE_DIR}/${CMAKE_ARCHIVE}
    else
        buildstep "CMake" echo "Using from existing source directory"
    fi
popd

pushd ${COMMON_BUILD_DIR}/${CMAKE_PACKAGE}
    buildstep "CMake" ./bootstrap --generator=${GENERATOR} --parallel=${CORES} --prefix=${COMMON_CROSS_DIR}
    buildstep "CMake" ninja -j ${CORES} all
    buildstep "CMake" ninja install
popd
