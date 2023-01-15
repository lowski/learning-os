#ifndef LEARNING_OS_SCHEDULING_H
#define LEARNING_OS_SCHEDULING_H

#define SCHEDULER_INTERVAL_MS 1000

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
    // r0-r12, sp (r13), lr (r12)
    unsigned int registers[15];
    void *pc;

    // 0 - idle thread, 1 - low, 2 - medium, 3 - high
    enum tcb_priority priority;
    struct signal* blocking_signal;
};

void scheduler_init(void);
struct tcb *scheduler(void *pc, unsigned int registers[15]);
void request_reschedule();

struct tcb *find_free_tcb();
struct tcb *find_tcb_by_id(unsigned int tid);
void kill_current_thread();
unsigned int get_current_thread_id();
struct tcb *get_current_tcb();

void block(unsigned int tid, struct signal *s);
void unblock(unsigned int tid);

void print_tcb(struct tcb* tcb);


#endif //LEARNING_OS_SCHEDULING_H
