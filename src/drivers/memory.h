#ifndef LEARNING_OS_MEMORY_H
#define LEARNING_OS_MEMORY_H

#include <stdint.h>

#define MEM_ADDR_MASTER_PAGE_TABLE (ptr_t) 0x21100000
#define MEM_ADDR_ALLOCATION_TABLE (ptr_t) 0x21110000
#define MEM_ADDR_SPECIAL_MODE_STACKS (ptr_t) 0x21200000
#define MEM_ADDR_TCBS (ptr_t) 0x21300000
#define MEM_ADDR_THREAD_STACKS (ptr_t) 0x21400000
#define MEM_PADDR_HEAP_START (ptr_t) 0x22000000

#define MEM_ADDR_SECTION_IVT (ptr_t) 0x00200000
#define MEM_ADDR_SECTION_TEXT_PRIVILEGED (ptr_t) 0x20000000
#define MEM_ADDR_SECTION_TEXT_USER (ptr_t) 0x20100000
#define MEM_ADDR_SECTION_DATA_USER (ptr_t) 0x20200000

#define MEM_LADDR_HEAP_START (ptr_t) 0x40000000

#define MEM_ADDR_PERIPHERY_AIC  (ptr_t) 0xFFFFF000 // advanced interrupt controller
#define MEM_ADDR_PERIPHERY_DBGU (ptr_t) 0xFFFFF200 // debug unit
#define MEM_ADDR_PERIPHERY_ST   (ptr_t) 0xFFFFFD00 // system timer

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
ptr_t mmu_get_fault_address();

ptr_t mem_malloc(uint32_t size);
void mem_free(ptr_t ptr);

#endif //LEARNING_OS_MEMORY_H
