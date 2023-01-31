#!/bin/zsh

PROJECT_PATH="$(cd -- "$(dirname "$0")" > /dev/null 2>&1 ; pwd -P)/"
cd $PROJECT_PATH

DOCKER_QEMU_COMMAND=(docker run -it -v "$(pwd)/kernel.elf:/kernel" --name qemu --rm ghcr.io/lowski/qemu-portux-920t:latest)
DOCKER_ARM_TOOLCHAIN_PREFIX=(docker run --user "$(id -u)":"$(id -g)" -v "$(pwd)":"$(pwd)" --rm --name make -w "$(pwd)" ghcr.io/lowski/gcc-arm-none-eabi arm-none-eabi)
DOCKER_MAKE_COMMAND=(docker run --user "$(id -u)":"$(id -g)" -v "$(pwd)":"$(pwd)" --rm --name make -w "$(pwd)" ghcr.io/lowski/gcc-arm-none-eabi make)

MAKE=make
ARM_TOOLCHAIN_PREFIX=arm-none-eabi
QEMU=qemu-system-arm

if [[ $(uname -s) == 'Darwin' ]]; then
  ARM_TOOLCHAIN_PREFIX=arm-unknown-linux-gnueabi
fi

USE_NATIVE_MAKE="$(command -v make)"
USE_NATIVE_ARM_TOOLCHAIN="$(command -v $ARM_TOOLCHAIN_PREFIX-gcc)"
USE_NATIVE_QEMU="$(command -v $QEMU)"

if [[ -n $USE_NATIVE_QEMU ]]; then
  USE_NATIVE_QEMU=$($QEMU -machine help | grep "portux920t")
fi

FORCE_DOCKER=false
if [[ $1 == '--docker' || $1 == '-d' ]]; then
  shift 1
  FORCE_DOCKER=true
fi

#echo $USE_NATIVE_MAKE
#echo $USE_NATIVE_QEMU
#echo $USE_NATIVE_ARM_TOOLCHAIN
#exit

if [[ -z "$USE_NATIVE_MAKE" ]] || [[ $FORCE_DOCKER == true ]]; then
  MAKE=(${DOCKER_MAKE_COMMAND[@]})
fi
if [[ -z "$USE_NATIVE_QEMU" ]] || [[ $FORCE_DOCKER == true ]]; then
  QEMU=(${DOCKER_QEMU_COMMAND[@]})
fi
if [[ -z "$USE_NATIVE_ARM_TOOLCHAIN" ]] || [[ $FORCE_DOCKER == true ]]; then
  ARM_TOOLCHAIN_PREFIX=(${DOCKER_ARM_TOOLCHAIN_PREFIX[@]})
fi

if [[ $@ == *' --clean' || $@ == *' -c' || $@ == *' -cm' ]]; then
  $MAKE clean || exit
fi
if [[ $@ == *' --make' || $@ == *' -m' || $@ == *' -cm' ]]; then
  $MAKE kernel.elf || exit
fi

if [[ ! -f "kernel.elf" ]]; then
  echo ""
  echo "No kernel.elf found. Exiting..."
  exit 1
fi

if [[ $1 == 'objdump' ]]; then
  shift 1
  $ARM_TOOLCHAIN_PREFIX-objdump -fhd kernel.elf $@
elif [[ $1 == 'qemu' ]]; then
  $QEMU
elif [[ $1 == 'qemu-gdb' ]]; then
  $QEMU -nographic -M portux920t -m 64M -kernel /kernel -S -gdb tcp::4444
elif [[ $1 == 'gdb' ]]; then
  $ARM_TOOLCHAIN_PREFIX-gdb -ex "target remote :4444" kernel.elf
else
  echo "Command \"$1\" not recognized. Supported: qemu, qemu-gdb, objdump, gdb."
fi