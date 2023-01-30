#include <stdio.h>

#include "swi.h"
#include "../drivers/aic.h"
#include "memory.h"

FUNC_PRIVILEGED
int handle_zero() {
    printf("Got SWI ZERO.\r\n");
    return 0;
}

FUNC_PRIVILEGED
int handle_set_irq(unsigned int enable) {
    set_irq_enabled(enable);
    return 0;
}

FUNC_PRIVILEGED
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
