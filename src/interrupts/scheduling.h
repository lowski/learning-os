#ifndef LEARNING_OS_SCHEDULING_H
#define LEARNING_OS_SCHEDULING_H

#include "../stdlib/datatypes.h"

#define SCHEDULER_INTERVAL_MS 100

enum tcb_status {
    not_existing,
    running,
    ready,
    blocked,
};

enum tcb_priority {
    idle,
    low,
    medium,
    high,
};

struct tcb {
    unsigned int id;
    unsigned int idx;
    void *sp_default;
    unsigned int cpsr;
    enum tcb_status status;
    // r0-r12, sp (r13), lr (r14)
    unsigned int registers[15];
    void *pc;

    // 0 - idle thread, 1 - low, 2 - medium, 3 - high
    enum tcb_priority priority;
    struct signal* blocking_signal;

    uint32_t reserved0[9]; // pad to 128 bytes length
};

/**
 * Initialize the scheduler:
 *  (1) set index and stack pointers for all TCBs
 *  (2) create idle thread
 *  (3) set the idle thread as current thread
 */
void scheduler_init(void);

/**
 * Save the context of the current thread and select the next thread. If there is no thread to select, the new
 * instruction pointer will be identical to the initial instruction pointer (`pc`).
 *
 * @param pc the current instruction pointer
 * @param registers a pointer to a list of all the registers (r0 to r14, with r0 at [0] and r14 at [14])
 * @return new instruction pointer
 */
void *scheduler(void *pc, unsigned int registers[15]);

/**
 * Schedule a new thread immediately. The time until the next scheduler call is reset, so the time slice for the new
 * thread is the normal size.
 */
void reschedule();

struct tcb *find_free_tcb();
struct tcb *find_tcb_by_id(unsigned int tid);
void kill_current_thread();

unsigned int get_current_thread_id();
struct tcb *get_current_tcb();

void block(unsigned int tid, struct signal *s);
void unblock(unsigned int tid);

void print_tcb(struct tcb* tcb);

#endif //LEARNING_OS_SCHEDULING_H
