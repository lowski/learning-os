#ifndef LEARNING_OS_SCHEDULING_H
#define LEARNING_OS_SCHEDULING_H

#define SCHEDULER_INTERVAL_MS 1000

enum tcb_status {
    not_existing,
    running,
    ready,
};

enum tcb_priority {
    idle,
    low,
    medium,
    high,
};

struct tcb {
    unsigned int id;
    void *sp_default;
    unsigned int cpsr;
    enum tcb_status status;
    // r0-r12, sp (r13), lr (r12)
    unsigned int registers[15];
    void *pc;

    // 0 - idle thread, 1 - low, 2 - medium, 3 - high
    enum tcb_priority priority;
};

unsigned int fork(void *pc);
void kill(unsigned int id);

void scheduler_init(void);
struct tcb *scheduler(void *pc, unsigned int registers[15]);

void print_tcb(struct tcb* tcb);


#endif //LEARNING_OS_SCHEDULING_H
