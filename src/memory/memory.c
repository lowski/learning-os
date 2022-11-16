//
// Created by Leonard von Lojewski on 15.11.22.
//

#include "memory.h"

#define MEMORY_CONTROLLER 0xFFFFFF00
static volatile unsigned int * const memory_controller = (unsigned int *)MEMORY_CONTROLLER;

#define SP_START 0x8FFFFFFF
#define SP_SIZE 512

#define MODE_MASK 0xFFFFFFE0
#define MODE_USR 0b10000
#define MODE_FIQ 0b10001
#define MODE_IRQ 0b10010
#define MODE_SVC 0b10011
#define MODE_ABT 0b10111
#define MODE_UND 0b11011
#define MODE_SYS 0b11111

void switch_mode(unsigned int mode) {
    register int cpsr;
    asm("MRS %0, CPSR" : "=r" (cpsr));
    cpsr &= MODE_MASK;
    cpsr |= mode;
    asm("MSR CPSR, %0" :: "r" (cpsr));
}

static inline void write_sp(unsigned int index) {
    asm("MOV %%sp, %0" :: "r" (SP_START - index * SP_SIZE));
}

void init_memory() {
    // Initialize IVT
    memory_controller[0] = 1;

    // Initialize stack pointers
    switch_mode(MODE_FIQ);
    write_sp(0);
    switch_mode(MODE_IRQ);
    write_sp(1);
    switch_mode(MODE_SVC);
    write_sp(2);
    switch_mode(MODE_ABT);
    write_sp(3);
    switch_mode(MODE_UND);
    write_sp(4);
    switch_mode(MODE_SYS);
    write_sp(5);

    // go back to the initial mode
    switch_mode(MODE_SVC);
}
