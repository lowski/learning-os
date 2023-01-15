#include "threading.h"
#include "../interrupts/scheduling.h"

unsigned int thread_count = 0;
unsigned int thread_id = 0;

void kill(unsigned int id) {
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