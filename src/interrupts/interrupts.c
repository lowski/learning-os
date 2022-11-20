//
// Created by Leonard von Lojewski on 15.11.22.
//

#include "interrupts.h"
#include "../serial_interface/serial.h"

#define INVALID_MEMORY 0x9000000
static volatile char* invalid_memory = (char*)INVALID_MEMORY;

void cause_data_abort() {
    *invalid_memory = 1;
}

void cause_software_interrupt() {
    asm("SWI 0");
}

void cause_undefined_instruction() {
    asm("LDMFD sp!,{pc}^");
}

__attribute__((naked, section(".handlers")))
void die() {
    asm("b die");
}

__attribute__((section(".handlers")))
void chandler_rst() {
    printf("There was an rst interrupt!\r\n");
}

__attribute__((section(".handlers")))
void *chandler_fiq(void *lr) {
    printf("There was a fiq interrupt!\r\n");

    // run both instructions that were already in the pipeline again
    return lr - 8;
}

__attribute__((section(".handlers")))
void *chandler_irq(void *lr) {
    printf("There was an irq interrupt!\r\n");

    // run both instructions that were already in the pipeline again
    return lr - 8;
}

__attribute__((section(".handlers")))
void *chandler_pfabt(void *lr) {
    printf("There was a prefetch abt interrupt!\r\n");
    printf("Location: %p\n", lr - 4);

    // skip the instruction that created the interrupt (which is one behind).
    return lr;
}

__attribute__((section(".handlers")))
void *chandler_dabt(void *lr) {
    printf("There was a data abt interrupt!\r\n");
    printf("Location: %p\n", lr - 8);

    // skip the instruction that created the interrupt (which is 2 behind)
    return lr - 4;
}

__attribute__((section(".handlers")))
void chandler_und(void *lr) {
    // a missing instruction means kill everything.

    printf("There was an undefined instruction!\r\n");
    printf("Location: %p\n", lr - 8);
    printf("Cannot recover - execution halted.\r\n");
    die();
}

__attribute__((section(".handlers")))
void *chandler_swi(void *lr) {
    // NOTE: If the SWI is caused from the SVC mode, the original LR is lost, and we will crash.
    printf("There was a swi interrupt!\r\n");
    printf("Location: %p\n", lr - 4);

    // return to the next instruction after the one that created the interrupt.
    return lr;
}