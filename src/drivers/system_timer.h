#ifndef LEARNING_OS_SYSTEM_TIMER_H
#define LEARNING_OS_SYSTEM_TIMER_H

#include <stdint.h>

#define SYSTEM_TIMER_PIT_INTERVAL_MS 100

void system_timer_init();

/**
 * Check if the period interval timer of the system timer has fired.
 * @return 1 or 0
 */
bit_t system_timer_status_pit();

#endif //LEARNING_OS_SYSTEM_TIMER_H
