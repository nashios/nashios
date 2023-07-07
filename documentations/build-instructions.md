# Build instructions

## Dependencies

The first step is check if you have all the required dependencies, follow the instructions for your system:

### Ubuntu

```bash
sudo apt install build-essential fuse2fs
```

### Fedora

```bash
sudo dnf install @"C Development Tools and Libraries"
```

### Windows

For windows is recommended to use [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) with [Ubuntu](https://www.microsoft.com/en-us/p/ubuntu/9nblggh4msv6?activetab=pivot:overviewtab) installed. Follow the microsoft install documentation and then use the ubuntu instructions. 

## CMake 3.26 is required

Check if the repository version is 3.26 or higher, if not use the following command:

```bash
meta/scripts/nashios.sh build cmake
```
_This command will download and compile the required cmake version_

## QEMU 8.0.2 is required

Check if the repository version is 8.0.2 or higher, if not use the following command:

```bash
meta/scripts/nashios.sh build qemu
```
_This command will download and compile the required qemu version_

## Toolchain

The first thing is to build the toolchain, you should choose the system architecture and the toolchain type. To see a list of all available options, run the following command:

```bash
meta/scripts/nashios.sh help
```

To build the toolchain, use the following command:

```bash
# --arch and --toolchain argumentes are optional (default: i686 and gnu)
meta/scripts/nashios.sh build toolchain --arch=<ARCH> --toolchain=<TOOLCHAIN>
```

After the toolchain is built, you can check for it in the `binary/install/toolchain/<ARCH>/<TOOLCHAIN>` directory. When you use `git pull` to update the repository, should be not required to build the toolchain again, it depends on the changes made on the following files/directories:

- **patches**: All .patch files located at `meta/patches` directory
- **libc**: All header files located at `libraries/c` directory

## System

For the system build, just run the following command:

```bash
# by default the `system` target is used
meta/scripts/nashios.sh build system
```

After the system is built, you can check for it in the `binary/install/system` directory, there should contain all the system binaries, libraries, etc.

## Running

Now you are able to run the system in a virtual machine, to do that, use the following command:

```bash
# by default the `qemu` is used
meta/scripts/nashios.sh run qemu
```

## Tips

For more commands or information all the commands have the `help` target, for example:

```bash
# this should prompt all build targets and options
meta/scripts/nashios.sh build help

# this should prompt all run targets and options
meta/scripts/nashios.sh run help

# for all script commands
meta/scripts/nashios.sh help
```