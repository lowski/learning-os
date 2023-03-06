# Learning OS

A complete operating system kernel for the Portux-920T (a dev-kit
computer used in education). The OS has no dependencies, everything is
written from scratch (only exception: stdarg.h for variadic functions).
It can be compiled using ARM GCC and runs on a QEMU emulator with a
custom patch for the Portux-920T.

Implemented Features:
- console I/O
- interrupt/exception handling
- multithreading/scheduling
- memory management/virtualization

## Running

Compile and run the kernel via QEMU: 
```shell
$ cd src
$ make run
```

Prerequisites are the `arm-none-eabi` toolchain and QEMU with the patch
for the Portux 920T.

## Setup

In order to run the microkernel, you need to install the toolchain to
compile for ARM and the QEMU patched with support for the Portux 920T.

### Docker

You can use a Docker image to run QEMU, if you don't want to install it
or it does not work on your system (e.g. Apple Silicon Mac).

You can either use the prebuilt image
`ghcr.io/lowski/qemu-portux-920t:latest` or build it yourself:
```shell
git clone git@git.imp.fu-berlin.de:koenigl/qemu-portux
cd qemu-portux && git submodule init && git submodule update && cd .. 
docker build -t qemu-portux-920 -f docker/qemu-portux-920/Dockerfile .
rm -rf qemu-portux
```

Then you can run the kernel:
```shell
cd src
docker run -it -v $(PWD)/kernel.elf:/kernel --name qemu --rm qemu-portux-920
```

### macOS toolchain install

```shell
brew tap messense/macos-cross-toolchains
brew install arm-unknown-linux-gnueabi
```

Install the patched QEMU for Portux920T support (won't work on M1):
```shell
brew install libffi gettext glib pkg-config pixman ninja # prerequisites for building qemu
git clone git@git.imp.fu-berlin.de/koenigl/qemu-portux
mkdir qemu-build && cd qemu-build
../qemu-portux/configure --target-list=arm-softmmu --disable-werror
alias nproc="sysctl -n hw.logicalcpu"
make -j$(nproc) # build multithreaded
```
