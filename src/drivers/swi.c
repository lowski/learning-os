//
// Created by Leonard von Lojewski on 15.01.23.
//

#include "swi.h"
#include "../stdlib/stdio.h"

int handle_zero() {
    printf("Got SWI ZERO.\r\n");
    return 0;
}

int handle_set_irq(unsigned int enable) {
    unsigned int value = (~enable) << 7;
    register int cpsr;
    asm("MRS %0, SPSR" : "=r" (cpsr));
    asm("MSR SPSR_c, %0" :: "r" ((cpsr & 0b01111111) | value));
    return 0;
}

int handle_swi(void *instruction_addr) {
    unsigned int instr = *((unsigned int *)instruction_addr);
    unsigned int code = instr & 0b11111111;

    switch (code) {
        case SWI_CODE_ZERO:
            return handle_zero();
        case SWI_CODE_DISABLE_IRQ:
            return handle_set_irq(0);
        case SWI_CODE_ENABLE_IRQ:
            return handle_set_irq(1);
        default:
            return 1;
    }
}
