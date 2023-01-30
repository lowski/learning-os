#include <stdio.h>
#include <thread.h>

#include "interrupts.h"
#include "../drivers/aic.h"
#include "../drivers/dbgu.h"
#include "../drivers/swi.h"
#include "../drivers/system_timer.h"
#include "scheduling.h"
#include "../drivers/memory.h"

extern unsigned int interrupt_demo_mode;

const unsigned int scheduler_max_interval = SCHEDULER_INTERVAL_MS / SYSTEM_TIMER_PIT_INTERVAL_MS;
unsigned int scheduler_ready = scheduler_max_interval;

unsigned int system_time_ms = 0;

unsigned int get_system_time_ms() {
    return system_time_ms;
}

extern struct tcb *TCBS;


__attribute__((naked, section(".handlers")))
void die() {
    asm("b die");
}

__attribute__((section(".handlers")))
void chandler_rst() {
    printf("There was an rst interrupt!\r\n");
}

__attribute__((section(".handlers")))
void *chandler_fiq(void *lr) {
    printf("There was a fiq interrupt!\r\n");

    // run both instructions that were already in the pipeline again
    return lr - 8;
}

FUNC_PRIVILEGED
void *chandler_irq(void *lr, unsigned int registers[15]) {
    aic_interrupt_handler_start();
    system_time_ms += SYSTEM_TIMER_PIT_INTERVAL_MS;
    unsigned int handled = 0;
    scheduler_ready--;

    if (aic_has_interrupted()) {
        // this means the interrupt originated with the AIC.
        // source 1 is the internal periphery.

        if (system_timer_status_pit()) {
            if (interrupt_demo_mode) {
                printf("!\r\n");
            }
            handled = 1;
        } else if (dbgu_status_rx_ready()) {
            dbgu_handle_irq();
            handled = 1;
        }
    }

    if (!handled) {
        printf("There was an unknown irq interrupt!\r\n");
    }

    void *next_pc = lr - 8;
    if (scheduler_ready <= 0) {
        scheduler_ready = scheduler_max_interval;
        next_pc = scheduler(next_pc, registers);
    }

    aic_interrupt_handler_finish();
    return next_pc;
}

__attribute__((section(".handlers")))
void *__chandler_irq_veneer(void *lr, unsigned int registers[15]) {
    return chandler_irq(lr, registers);
}

__attribute__((section(".handlers")))
void *chandler_pfabt(void *lr) {
    printf("There was a prefetch abt interrupt!\r\n");
    printf("Location: %p\n", lr - 4);

    // skip the instruction that created the interrupt (which is one behind).
    return lr;
}

__attribute__((section(".handlers")))
void *chandler_dabt(void *lr) {
    printf("There was a data abt interrupt!\r\n");
    printf("Location: %p\n", lr - 8);

    // skip the instruction that created the interrupt (which is 2 behind)
    return lr - 4;
}

__attribute__((section(".handlers")))
void chandler_und(void *lr) {
    // a missing instruction means kill everything.

    printf("There was an undefined instruction!\r\n");
    printf("Location: %p\n", lr - 4);
    printf("Cannot recover - execution halted.\r\n");
    die();
}

FUNC_PRIVILEGED
void *chandler_swi(void *lr, unsigned int registers[15]) {
    // NOTE: If the SWI is caused from the SVC mode, the original LR is lost, and we will crash.
    set_irq_enabled(0);
    unsigned int code = *(unsigned int *)(lr - 4) & 255;

    if (code == SWI_CODE_RESCHEDULE) {
        scheduler_ready = scheduler_max_interval;

        void *next_pc = scheduler(lr, registers);
        set_irq_enabled(1);
        return next_pc;
    }

    int not_handled = handle_swi(lr - 4);
    if (not_handled) {
        printf("There was an unknown swi interrupt!\r\n");
        printf("Location: %p\n", lr - 4);
        printf("Code: %d\n", *(unsigned int *)(lr - 4) & 255);
    }

    // return to the next instruction after the one that created the interrupt.
    set_irq_enabled(1);
    return lr;
}

__attribute__((section(".handlers")))
void *__chandler_swi_veneer(void *lr, unsigned int registers[15]) {
    return chandler_swi(lr, registers);
}
