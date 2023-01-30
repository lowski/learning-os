#include <stdint.h>

#include "system_timer.h"
#include "memory.h"

struct system_timer {
    uint32_t control_register; // WO
    uint32_t period_interval_mode; // RW
    uint32_t watchdog_mode; // RW
    uint32_t real_time_mode; // RW
    struct {
        bit_t period_interval_timer: 1;
        bit_t watchdog: 1;
        bit_t real_time_timer: 1;
        bit_t alarm: 1;
        uint_t: 4;

        uint8_t _reserved0[3];
    } status; // RO
    uint32_t interrupt_enable; // WO
    uint32_t interrupt_disable; // WO
    uint32_t interrupt_mask; // RO
    uint32_t real_time_alarm; // RW
    uint32_t current_real_time; // RO
};

static volatile struct system_timer *const system_timer = (struct system_timer *)MEM_ADDR_PERIPHERY_ST;

#define SYSTEM_TIMER_MASK_PITS 0b1
#define SYSTEM_TIMER_MASK_WATCHDOG 0b10
#define SYSTEM_TIMER_MASK_RTT 0b100
#define SYSTEM_TIMER_MASK_ALARM 0b1000

// AT91RM9200 manual section 24.4.1
#define SYSTEM_TIMER_CLOCK_HZ 32768

static const unsigned int SYSTEM_TIMER_PIT_COUNTER = SYSTEM_TIMER_CLOCK_HZ * SYSTEM_TIMER_PIT_INTERVAL_MS / 1000;

FUNC_PRIVILEGED
void system_timer_init() {
    system_timer->period_interval_mode = SYSTEM_TIMER_PIT_COUNTER;
    system_timer->interrupt_enable = SYSTEM_TIMER_MASK_PITS;
}

FUNC_PRIVILEGED
bit_t system_timer_status_pit() {
    return system_timer->status.period_interval_timer;
}
