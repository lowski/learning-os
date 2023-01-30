#include "aic.h"
#include "memory.h"
#include <stdint.h>

struct aic {
    uint32_t source_mode[32]; // RW
    uint32_t source_vector[32]; // RW

    uint32_t interrupt_vector; // RO
    uint32_t fast_interrupt_vector; // RO
    uint32_t interrupt_status; // RO
    uint32_t interrupt_pending; // RO
    uint32_t interrupt_mask; // RO
    uint32_t core_interrupt_status; // RO
    uint32_t: 32;
    uint32_t: 32;

    uint32_t interrupt_enable_command; // WO
    uint32_t interrupt_disable_command; // WO
    uint32_t interrupt_clear_command; // WO
    uint32_t interrupt_set_command; // WO
    uint32_t end_of_interrupt_command; // WO
    uint32_t spurious_interrupt_vector; // RW
    uint32_t debug_control; // RW

    uint32_t: 32;
};

static volatile struct aic *const aic = (struct aic *)MEM_ADDR_PERIPHERY_AIC;

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

FUNC_PRIVILEGED
void aic_interrupt_handler_start() {
    aic->interrupt_vector;
}

FUNC_PRIVILEGED
void aic_interrupt_handler_finish() {
    aic->end_of_interrupt_command = 1;
}

FUNC_PRIVILEGED
uint32_t aic_has_interrupted() {
    return aic->interrupt_status == 1;
}