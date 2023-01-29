#include "threading.h"
#include "../interrupts/scheduling.h"
#include "../interrupts/interrupts.h"
#include "../drivers/memory.h"

unsigned int thread_id = 0;

void kill(unsigned int id) {
    struct tcb *t = find_tcb_by_id(id);
    if (t == 0) return;

    t->status = not_existing;
    if (t->status == running) {
        reschedule();
    }
}

unsigned int clone(void *pc) {
    struct tcb *t = find_free_tcb();
    if (t == 0) {
        return 0;
    }
    t->id = thread_id++;
    t->status = ready;
    t->pc = pc;
    t->priority = medium;
    t->cpsr = 0b10000;
    t->registers[13] = (unsigned int)t->sp_default;
    t->registers[14] = (unsigned int)kill_current_thread;

    return t->id;
}

void signal(struct signal *s) {
    s->unblocked = 1;
    if (s->blocked_tid != 0) {
        unblock(s->blocked_tid);
        s->blocked_tid = 0;
    }
}

void wait(struct signal *s) {
    if (s->unblocked == 1) {
        s->unblocked = 0;
        return;
    }
    unsigned int tid = get_current_thread_id();
    s->blocked_tid = tid;
    block(tid, s);
    s->unblocked = 0;
}

unsigned int last_sleep_thread_system_time = 0;
int sleeping_ms[MAX_THREAD_COUNT] = {0};
struct signal sleeping_signals[MAX_THREAD_COUNT] = {};

void sleeping_beauty() {
    for (;;) {
        unsigned int current_time = get_system_time_ms();
        unsigned int delta_ms = current_time - last_sleep_thread_system_time;
        last_sleep_thread_system_time = current_time;

        for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
            if (sleeping_ms[i] > 0) {
                sleeping_ms[i] -= delta_ms;
            }
            if (sleeping_ms[i] <= 0) {
                signal(&sleeping_signals[i]);
            }
        }
        reschedule();
    }
}

void sleep(unsigned int ms) {
    unsigned int idx = get_current_tcb()->idx;
    sleeping_ms[idx] = ms;
    (&sleeping_signals[idx])->unblocked = 0;
    wait(&sleeping_signals[idx]);
}

FUNC_PRIVILEGED
void threading_init() {
    clone(sleeping_beauty);
}