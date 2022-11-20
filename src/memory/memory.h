#ifndef LEARNING_OS_MEMORY_H
#define LEARNING_OS_MEMORY_H

#define MODE_USR 0b10000 // user
#define MODE_FIQ 0b10001 // fast interrupt
#define MODE_IRQ 0b10010 // interrupt
#define MODE_SVC 0b10011 // supervisor
#define MODE_ABT 0b10111 // data abort
#define MODE_UND 0b11011 // undefined instruction
#define MODE_SYS 0b11111 // system

#define switch_mode(mode) asm("MSR CPSR_c, %0" :: "r" (mode));

void init_memory();
unsigned int get_current_mode();

#endif //LEARNING_OS_MEMORY_H
