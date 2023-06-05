# Build instructions

## Prerequisites

Before compiling the system, make sure you have the following packages installed

### Debian family

```console
# CMake
sudo apt install cmake ninja-build
# GCC
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
# Virtual machine
sudo apt install qemu-system-x86 fuse2fs
```

### Fedora

```console
# CMake
sudo dnf install cmake ninja-build
# GCC
sudo dnf install texinfo binutils-devel mpfr-devel libmpc-devel gmp-devel e2fsprogs @"C Development Tools and Libraries"
# Virtual machine
sudo dnf install @Virtualization
```

## Build

The first step is to generate a cross-compiler, so we can separate the headers, libraries from the host operating system. The following command will compile a toolchain based on the selected architecture and target the Nashi operating system.

```console
Meta/Scripts/NashiOS.sh toolchain
```

_When the compilation is finished, we will have an `i686-pc-nashios` toolchain ready to use on the system._

Now we can compile all programs, libraries and the Kernel itself, using the following command:

```console
Meta/Scripts/NashiOS.sh build
```

In order to be able to use the operating system in some emulator, we have to install the system somewhere, in this case in the `Binary` folder itself, the following command will generate another folder called `Sysroot` which will contain all the final files of the system, with the organization of system-specific folders.

```console
Meta/Scripts/NashiOS.sh install
```

_If you are curious, visit the `Binary/Build/Architecture/i686/gnu/Sysroot` folder, you will see a similarity with Linux, this is not by chance, we call ourselves [Unix-like](https://en.wikipedia.org/wiki/Unix-like) or rather, followers of the [POSIX](https://en.wikipedia.org/wiki/POSIX) standard._

## Run

In order to boot the system, we need to generate an image with the ext2 file system containing all the necessary binaries for the system to work! Sounds complicated but we have a script for that, run it now:

```console
Meta/Scripts/NashiOS.sh image
```

The great moment of testing the system has arrived, for this we use the [QEMU](https://en.wikipedia.org/wiki/QEMU) emulator that will serve as a virtualizer of a real machine, so we can debug and test the system in a simple and fast way without having to configure a physical machine, but if you want to test the famous bare-metal, feel free.

```console
Meta/Scripts/NashiOS.sh run
```

## Final notes

When there are updates to the code you won't need to go through the whole process again, just run through the `install`, `image` and `run` steps. You will rarely need to compile the `toolchain` again, these are specific cases and you will know if you need to recompile.

Be sure to check the `NashiOS.sh` script commands in the `Meta/Scripts` folder, it is the main command used throughout the project!
