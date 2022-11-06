# Learning OS

Compile and run the kernel via QEMU: 
```shell
$ cd src
$ make run
```

Prerequisites are the `arm-none-eabi` toolchain and QEMU with the patch
for the Portux 920T.

## macOS toolchain install

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
