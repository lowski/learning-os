# Quellen
LSCRIPT = kernel.lds
#SRC_FILES = start.c main.c serial_interface/serial.c stdlib/str.c
#OBJ = $(SRC_FILES:.c:.o)
OBJ = start.o main.o serial_interface/serial.o stdlib/str.o
DEP = $(OBJ:.o=.d)

#CC = arm-none-eabi-gcc
#LD = arm-none-eabi-ld
#OBJCOPY = arm-none-eabi-objcopy
CC := arm-unknown-linux-gnueabi-gcc
LD := arm-unknown-linux-gnueabi-ld
OBJCOPY := arm-unknown-linux-gnueabi-objcopy

CFLAGS = -Wall -Wextra -ffreestanding -mcpu=arm920t -O2

.PHONY: all
all: kernel

-include $(DEP)

%.o: %.S
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -o $@ -c $<

kernel.elf: $(LSCRIPT) $(OBJ)
	$(LD) -T$(LSCRIPT) -o $@ $(OBJ) $(LIBGCC)

.PHONY: clean
clean:
	rm -f kernel kernel.bin kernel.img
	rm -f $(OBJ)
	rm -f $(DEP)
