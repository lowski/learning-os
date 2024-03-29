# ============
# SOURCE FILES
# ============

LSCRIPT = src/kernel.lds
DIRS = src/interrupts src/memory src/serial_interface src/lib src/drivers src/application
INCLUDE = include/

SRC_C = $(wildcard *.c $(foreach fd, $(DIRS), $(fd)/*.c)) src/start.c
SRC_S = $(wildcard *.S $(foreach fd, $(DIRS), $(fd)/*.S))
OBJ = $(SRC_C:.c=.o) $(SRC_S:.S=.o)
DEP = $(OBJ:.o=.d)

# ======================
# COMPILER CONFIGURATION
# ======================

CFLAGS = -Wall -Wextra -Wno-packed-bitfield-compat -ffreestanding -mcpu=arm920t -O2 -std=c11 -g -fasm

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

ifeq ($(shell uname -s), Darwin)
	CC = arm-unknown-linux-gnueabi-gcc
	LD = arm-unknown-linux-gnueabi-ld
	OBJCOPY = arm-unknown-linux-gnueabi-objcopy
endif

# =======
# TARGETS
# =======

-include $(DEP)

%.o: %.S
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -o $@ -nostdinc -isystem $(INCLUDE) -c $<

kernel.elf: $(LSCRIPT) $(OBJ)
	$(LD) -T$(LSCRIPT) -o $@ $(OBJ) $(LIBGCC)

.PHONY: all
all: kernel.elf

.PHONY: run
run: kernel.elf
	qemu-system-arm -nographic -M portux920t -m 64M -kernel $<

.PHONY: clean
clean:
	rm -f kernel.elf kernel.bin kernel.img
	rm -f $(OBJ)
	rm -f $(DEP)
