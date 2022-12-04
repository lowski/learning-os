#include "system_timer.h"

// AT91RM9200 manual section 24.4.1
#define SYSTEM_TIMER_CLOCK_HZ 32768
#define SYSTEM_TIMER_PIT_INTERVAL_MS 100

static const unsigned int SYSTEM_TIMER_PIT_COUNTER = SYSTEM_TIMER_CLOCK_HZ * SYSTEM_TIMER_PIT_INTERVAL_MS / 1000;

void system_timer_init() {
    system_timer->period_interval_mode = SYSTEM_TIMER_PIT_COUNTER;
    system_timer->interrupt_enable = SYSTEM_TIMER_MASK_PITS;
}
