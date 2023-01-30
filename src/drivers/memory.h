#ifndef LEARNING_OS_MEMORY_H
#define LEARNING_OS_MEMORY_H

#define MEM_ADDR_MASTER_PAGE_TABLE 0x21100000
#define MEM_ADDR_SPECIAL_MODE_STACKS 0x21200000
#define MEM_ADDR_TCBS 0x21300000
#define MEM_ADDR_THREAD_STACKS 0x21400000

#define MEM_ADDR_SECTION_IVT 0x00200000
#define MEM_ADDR_SECTION_TEXT_PRIVILEGED 0x20000000
#define MEM_ADDR_SECTION_TEXT_USER 0x20100000
#define MEM_ADDR_SECTION_DATA_USER 0x20200000

#define MEM_ADDR_PERIPHERY_AIC  0xFFFFF000 // advanced interrupt controller
#define MEM_ADDR_PERIPHERY_DBGU 0xFFFFF200 // debug unit
#define MEM_ADDR_PERIPHERY_ST   0xFFFFFD00 // system timer

#define MEM_SIZE_SPECIAL_MODE_STACK 0x200
#define MEM_SIZE_THREAD_STACK 0x200

#define MODE_USR 0b10000 // user
#define MODE_FIQ 0b10001 // fast interrupt
#define MODE_IRQ 0b10010 // interrupt
#define MODE_SVC 0b10011 // supervisor
#define MODE_ABT 0b10111 // data abort
#define MODE_UND 0b11011 // undefined instruction
#define MODE_SYS 0b11111 // system

#define switch_mode(mode) asm("MSR CPSR_c, %0" :: "r" (mode))

#define FUNC_PRIVILEGED __attribute__((section(".text_privileged")))

void init_memory();
unsigned int get_current_mode();

#endif //LEARNING_OS_MEMORY_H
