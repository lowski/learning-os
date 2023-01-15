#ifndef LEARNING_OS_SYSTEM_TIMER_H
#define LEARNING_OS_SYSTEM_TIMER_H

struct system_timer {
    unsigned int control_register; // WO
    unsigned int period_interval_mode; // RW
    unsigned int watchdog_mode; // RW
    unsigned int real_time_mode; // RW
    struct {
        unsigned int period_interval_timer: 1;
        unsigned int watchdog: 1;
        unsigned int real_time_timer: 1;
        unsigned int alarm: 1;
        unsigned int unused1: 4;

        unsigned char unused0[3];
    } status; // RO
    unsigned int interrupt_enable; // WO
    unsigned int interrupt_disable; // WO
    unsigned int interrupt_mask; // RO
    unsigned int real_time_alarm; // RW
    unsigned int current_real_time; // RO
};

#define SYSTEM_TIMER_MASK_PITS 0b1
#define SYSTEM_TIMER_MASK_WATCHDOG 0b10
#define SYSTEM_TIMER_MASK_RTT 0b100
#define SYSTEM_TIMER_MASK_ALARM 0b1000

#define SYSTEM_TIMER_PIT_INTERVAL_MS 100

#define SYSTEM_TIMER_ADDR 0xFFFFFD00
static volatile struct system_timer *const system_timer = (struct system_timer *)SYSTEM_TIMER_ADDR;

void system_timer_init();

#endif //LEARNING_OS_SYSTEM_TIMER_H
