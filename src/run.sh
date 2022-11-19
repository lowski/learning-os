#!/bin/zsh

PROJECT_PATH="$(cd -- "$(dirname "$0")" > /dev/null 2>&1 ; pwd -P)/"
cd $PROJECT_PATH

if [[ $@ == *' --clean' || $@ == *' -c' || $@ == *' -cm' ]]; then
  make clean || exit
fi
if [[ $@ == *' --make' || $@ == *' -m' || $@ == *' -cm' ]]; then
  make kernel.elf || exit
fi

if [[ $1 == 'objdump' ]]; then
  shift 1
  arm-unknown-linux-gnueabi-objdump -fhd kernel.elf $@
elif [[ $1 == 'docker' ]]; then
  docker run -it -v "$(pwd)/kernel.elf":/kernel --name qemu --rm qemu-runner
elif [[ $1 == 'docker-gdb' ]]; then
  docker run -it -p 4444:4444 -v "$(pwd)/kernel.elf":/kernel --name qemu --rm qemu-runner -nographic -M portux920t -m 64M -kernel /kernel -S -gdb tcp::4444
elif [[ $1 == 'gdb' ]]; then
  arm-unknown-linux-gnueabi-gdb -ex "target remote :4444" kernel.elf
else
  echo "Command \"$1\" not recognized. Supported: docker, docker-gdb, gdb."
fi