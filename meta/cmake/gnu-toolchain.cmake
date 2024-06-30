set(CMAKE_SYSTEM_NAME NashiOS)
set(CMAKE_SYSTEM_PROCESSOR ${SYSTEM_ARCH})

set(TOOLCHAIN ${TOOLCHAIN_PATH}/bin/${TOOLCHAIN_TARGET})

set(CMAKE_SYSROOT ${SYSTEM_PATH})
set(CMAKE_STAGING_PREFIX ${SYSTEM_PATH})
set(CMAKE_INSTALL_PREFIX /usr/local)
set(CMAKE_INSTALL_DATAROOTDIR share)

set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_LINKER ${TOOLCHAIN}-ld)
set(CMAKE_AR ${TOOLCHAIN}-gcc-ar)
set(CMAKE_NM ${TOOLCHAIN}-gcc-nm)
set(CMAKE_OBJCOPY ${TOOLCHAIN}-objcopy)
set(CMAKE_RANLIB ${TOOLCHAIN}-gcc-ranlib)
set(CMAKE_STRIP ${TOOLCHAIN}-strip)

set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)