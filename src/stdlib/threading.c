#include "threading.h"
#include "../interrupts/scheduling.h"
#include "../interrupts/interrupts.h"
#include "../stdlib/stdio.h"
#include "../stdlib/str.h"

unsigned int thread_count = 0;
unsigned int thread_id = 0;

void kill(unsigned int id) {
    printf("kill %d\n", id);
    struct tcb *t = find_tcb_by_id(id);
    if (t == 0) return;

    // We ignore if the thread is currently running. It will not be stopped immediately, but will not be scheduled
    // again.
    t->status = not_existing;
}

unsigned int fork(void *pc) {
    if (thread_count >= MAX_THREAD_COUNT) {
        return 0;
    }
    thread_count++;

    struct tcb *t = find_free_tcb();
    if (t == 0) {
        return 0;
    }
    t->id = thread_id++;
    t->status = ready;
    t->pc = pc;
    t->priority = medium;
    t->registers[13] = (unsigned int)t->sp_default;
    t->registers[14] = (unsigned int)kill_current_thread;

    return t->id;
}

void signal(struct signal *s) {
    if (s->blocked_tid != 0) {
        unblock(s->blocked_tid);
        s->blocked_tid = 0;
    }
}

void wait(struct signal *s) {
    unsigned int tid = get_current_thread_id();
    s->blocked_tid = tid;
    block(tid, s);
}

unsigned int last_sleep_thread_system_time = 0;
int sleeping_ms[MAX_THREAD_COUNT] = {0};
struct signal sleeping_signals[MAX_THREAD_COUNT] = {};

void sleep_managing_thread() {
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
        for (int j = 0; j < 50000000; ++j) asm("NOP");
        request_reschedule();
    }
}

void sleep(unsigned int ms) {
    unsigned int idx = get_current_tcb()->idx;
    sleeping_ms[idx] = ms;
    wait(&sleeping_signals[idx]);
}

void threading_init() {
    fork(sleep_managing_thread);
}