#include <stdio.h>

#include "swi.h"
#include "../drivers/aic.h"
#include "memory.h"

FUNC_PRIVILEGED
void handle_zero() {
    printf("Got SWI ZERO.\r\n");
}

FUNC_PRIVILEGED
void handle_set_irq(unsigned int enable) {
    set_irq_enabled(enable);
}

FUNC_PRIVILEGED
void handle_swi(void *instruction_addr, uint32_t registers[15]) {
    unsigned int instr = *((unsigned int *)instruction_addr);
    unsigned int code = instr & 0b11111111;

    uint32_t res = registers[0];
    switch (code) {
        case SWI_CODE_ZERO:
            return handle_zero();
        case SWI_CODE_DISABLE_IRQ:
            return handle_set_irq(0);
        case SWI_CODE_ENABLE_IRQ:
            return handle_set_irq(1);
        case SWI_CODE_MEM_MALLOC:
            res = (uint32_t) mem_malloc(registers[0]);
            break;
        case SWI_CODE_MEM_FREE:
            mem_free((ptr_t) registers[0]);
            break;
        default:
            printf("There was an unknown swi interrupt!\r\n");
            printf("Location: %p\n", instruction_addr);
            printf("Code: %d\n", code);
            return;
    }
    registers[0] = res;
}
