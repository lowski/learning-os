#ifndef LEARNING_OS_INTERRUPTS_H
#define LEARNING_OS_INTERRUPTS_H

void cause_data_abort();
void cause_software_interrupt();
void cause_undefined_instruction();

void demo_interrupts();
void demo_fork();

#endif //LEARNING_OS_INTERRUPTS_H
