#ifndef LEARNING_OS_AIC_H
#define LEARNING_OS_AIC_H

struct aic {
    unsigned int source_mode[32]; // RW
    unsigned int source_vector[32]; // RW

    unsigned int interrupt_vector; // RO
    unsigned int fast_interrupt_vector; // RO
    unsigned int interrupt_status; // RO
    unsigned int interrupt_pending; // RO
    unsigned int interrupt_mask; // RO
    unsigned int core_interrupt_status; // RO
    unsigned int unused0[2];

    unsigned int interrupt_enable_command; // WO
    unsigned int interrupt_disable_command; // WO
    unsigned int interrupt_clear_command; // WO
    unsigned int interrupt_set_command; // WO
    unsigned int end_of_interrupt_command; // WO
    unsigned int spurious_interrupt_vector; // RW
    unsigned int debug_control; // RW

    unsigned int unused1;
};

#define ADDR_ADVANCED_INTERRUPT_CONTROLLER 0xFFFFF000
static volatile struct aic *const aic = (struct aic *)ADDR_ADVANCED_INTERRUPT_CONTROLLER;

void aic_init();
void set_irq_enabled(unsigned int enable);

#endif //LEARNING_OS_AIC_H
