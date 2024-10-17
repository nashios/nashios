ARCH=x86
TARGET=i686-elf
CORES=$(nproc)

SOURCE_DIR=$(git rev-parse --show-toplevel)
BUILD_DIR=${SOURCE_DIR}/binary/build
INSTALL_DIR=${SOURCE_DIR}/binary/install

args=()
args+=("--toolchain ${SOURCE_DIR}/meta/cmake/gnu-toolchain.cmake")
args+=("--install-prefix ${INSTALL_DIR}/system/${ARCH}")
args+=("-D TOOLCHAIN_PATH=${INSTALL_DIR}/toolchain/${ARCH}")
args+=("-D TOOLCHAIN_TARGET=${TARGET}")
args+=("-D SYSTEM_PATH=${INSTALL_DIR}/system/${ARCH}")
args+=("-D SYSTEM_ARCH=${ARCH}")

cmake -S ${SOURCE_DIR} -B ${BUILD_DIR}/system/${ARCH} ${args[@]}
cmake --build ${BUILD_DIR}/system/${ARCH}
cmake --install ${BUILD_DIR}/system/${ARCH}

qemu-system-i386 -kernel ${INSTALL_DIR}/system/${ARCH}/boot/kernel -serial stdio