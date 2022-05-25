# NashiOS

Unix-like operating system for x86 architecture.

[![Build](https://github.com/nashios/nashios/actions/workflows/cmake.yml/badge.svg)](https://github.com/nashios/nashios/actions/workflows/cmake.yml)

![Screenshot](https://github.com/nashios/nashios/blob/main/meta/images/screenshot-2022-05-24-175408.png?raw=true)
*NashiOS boot time*

## About

NashiOS is a hobby operating system with a Unix-like core, initially designed for the x86 architecture, but with future
plans to be ported to other platforms. Its name comes from the pear-nashi,
nashi [(Pyrus pyrifolia)](https://en.wikipedia.org/wiki/Pyrus_pyrifolia) and is sometimes called the Asian pear. Nashi
pears are widely cultivated for their sweet fruit, a popular food in East Asia.

## Build

### Prerequisites

We need some dependencies installed to be able to generate the cross compiler and emulate the system.

#### Debian family

```bash
# GCC dependencies
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo 
# Build system
sudo apt install cmake ninja-build 
# Emulator
sudo apt install qemu-system-x86
```

### Compile

#### Configure build system

First we need to generate the `build` folder at the root of the project:

```bash
mkdir build
cd build
```

Then we will configure the project and generate the build system:

```bash
# The -G argument specify a build system
cmake .. -G Ninja
```

We use [CMake](https://cmake.org/) to build and configure our build system and [ninja-build](https://ninja-build.org/)
as the build system.

### Build the project

To compile NashiOS, you first need to build the toolchain by running the following command:

```bash
ninja toolchain
```

This step is only done the first time and when there are changes in critical parts of the system.

Now it's time to compile the entire system:

```bash
ninja install
```

This will compile the entire system and install the built files into the `sysroot` directory.

### Running

With the system compiled without errors, let's run it in the [QEMU](https://www.qemu.org/) emulator:

```bash
ninja run
```

Later, when you use git pull to get the latest changes, you will again use the above commands to verify the changes.

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## Author

* **Saullo Bretas** - [saullo](https://github.com/saullo)

## License

[GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/) - GNU General Public License v3.0
