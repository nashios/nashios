# Build instructions

## Prerequisites

Before compiling the system, make sure you have the following packages installed

### Debian family

```console
# Cmake related packages
sudo apt install cmake ninja-build
# Gcc related packages
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo
# Virtual machine
sudo apt install qemu-system-x86
```

## Build

The first step is to generate the cmake build files. Replace `arch` in the next commands with some supported architecture, if in doubt which one, check the [CMakePresets.json](/CMakePresets.json) file or use `i686` to start your tests.

```console
cmake --preset arch
```

*This command will generate a `binary` folder in the root of the project, containing several files needed to compile the system*

After generating the basic files, we can start the compilation process. We will need a cross-compiler, so we can separate the headers, libraries from the host operating system. The following command will compile a toolchain based on the selected architecture and target the Nashi operating system.

```console
cmake --build --preset arch --target toolchain
```

*When the compilation is finished, we will have an `arch-elf` toolchain ready to use on the system.*

Now we can compile all programs, libraries and the Kernel itself, using the following command:

```console
cmake --build --preset arch --target all
```

In order to be able to use the operating system in some emulator, we have to install the system somewhere, in this case in the `binary` folder itself, the following command will generate another folder called `sysroot` which will contain all the final files of the system, with the organization of system-specific folders.

```console
cmake --build --preset arch --target install
```

*If you are curious, visit the `binary/build/arch/sysroot` folder, you will see a similarity with Linux, this is not by chance, we call ourselves [Unix-like](https://en.wikipedia.org/wiki/Unix-like) or rather, followers of the [POSIX](https://en.wikipedia.org/wiki/POSIX) standard.*

## Run

In order to boot the system, we need to generate an image with the ext2 file system containing all the necessary binaries for the system to work! Sounds complicated but we have a script for that, run it now:

```console
cmake --build --preset arch --target image
```

The great moment of testing the system has arrived, for this we use the [QEMU](https://en.wikipedia.org/wiki/QEMU) emulator that will serve as a virtualizer of a real machine, so we can debug and test the system in a simple and fast way without having to configure a physical machine, but if you want to test the famous bare-metal, feel free. 

```console
cmake --build --preset arch --target run
```

## Final notes

When there are updates to the code you won't need to go through the whole process again, just run through the `install`, `image` and `run` steps. You will rarely need to compile the `toolchain` again, these are specific cases and you will know if you need to recompile.
