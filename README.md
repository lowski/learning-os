# Learning OS

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
