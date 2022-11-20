#include "memory.h"

#define MEMORY_CONTROLLER 0xFFFFFF00
static volatile unsigned int * const memory_controller = (unsigned int *)MEMORY_CONTROLLER;

#define SP_START 0x002FFFFF
#define SP_SIZE 512
#define write_sp(index) asm("MOV %%sp, %0" :: "r" (SP_START - index * SP_SIZE));

#define MODE_MASK 0xFFFFFFE0

unsigned int get_current_mode() {
    register int cpsr;
    asm("MRS %0, CPSR" : "=r" (cpsr));
    return cpsr & (~MODE_MASK);
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

    // user mode is ignored for now, as we won't get out anymore :/

    // Go to user mode and initialize stack pointer
    switch_mode(MODE_SVC);
//    switch_mode(MODE_USR);
}
