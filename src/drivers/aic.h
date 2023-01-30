#ifndef LEARNING_OS_AIC_H
#define LEARNING_OS_AIC_H

#include <stdint.h>

void aic_init();
void set_irq_enabled(unsigned int enable);

/**
 * Notify the AIC that an interrupt handler has started. This will block other interrupts until
 * aic_interrupt_handler_finish() is called. Internally this will read the interrupt vector register.
 */
void aic_interrupt_handler_start();
/**
 * Notify the AIC that an interrupt handler has finished.
 */
void aic_interrupt_handler_finish();
/**
 * Check whether the AIC has triggered an interrupt.
 * @return 0 or 1
 */
uint32_t aic_has_interrupted();

#endif //LEARNING_OS_AIC_H
