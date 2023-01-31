#ifndef LEARNING_OS_SWI_H
#define LEARNING_OS_SWI_H

#define SWI_CODE_ZERO 0
#define SWI_CODE_DISABLE_IRQ 1
#define SWI_CODE_ENABLE_IRQ 2
#define SWI_CODE_RESCHEDULE 3
#define SWI_CODE_MEM_MALLOC 10
#define SWI_CODE_MEM_FREE 11

#define raise_swi(code) asm("SWI %0" :: "i"(code))

void handle_swi(void *instruction_addr, unsigned int registers[15]);

#endif //LEARNING_OS_SWI_H
