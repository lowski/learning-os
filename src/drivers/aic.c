#include "aic.h"
#include "memory.h"

FUNC_PRIVILEGED
void aic_init() {
    // enable interrupts on channel 1 (internal periphery).
    aic->interrupt_enable_command = 0b10;

    // set source 1 to positive-edge triggered.
    aic->source_mode[1] = 1 << 5;
}

FUNC_PRIVILEGED
void set_irq_enabled(unsigned int enable) {
    unsigned int value = (~enable) << 7;
    register int cpsr;
    asm("MRS %0, SPSR" : "=r" (cpsr));
    asm("MSR SPSR_c, %0" :: "r" ((cpsr & 0b01111111) | value));
}