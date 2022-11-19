# ============
# SOURCE FILES
# ============

LSCRIPT = kernel.lds
OBJ = start.o main.o serial_interface/serial.o stdlib/str.o interrupts/interrupts.o memory/memory.o interrupts/ivt.o
DEP = $(OBJ:.o=.d)

# ======================
# COMPILER CONFIGURATION
# ======================

CFLAGS = -Wall -Wextra -ffreestanding -mcpu=arm920t -O2 -std=c11 -g -fasm

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
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

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
