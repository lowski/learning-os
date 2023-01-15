#include "scheduling.h"
#include "../drivers/swi.h"
#include "../stdlib/str.h"
#include "../stdlib/stdio.h"

#define TCB_START 0x2F0000
#define MAX_THREAD_COUNT 16
#define THREAD_STACK_SIZE 512
#define THREAD_STACK_START 0x2FF3FF

unsigned int thread_count = 0;
unsigned int thread_id = 0;
unsigned int current_thread_idx = 0;
struct tcb *TCBS = (struct tcb *)TCB_START;
struct tcb *current_thread;

void thread_nop() {
    for (;;) asm("NOP");
}

struct tcb *find_free_tcb() {
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        if (TCBS[i].status == not_existing) {
            return &TCBS[i];
        }
    }
    return 0;
}

struct tcb *find_tcb_by_id(unsigned int id) {
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        if (TCBS[i].id == id) {
            return &TCBS[i];
        }
    }
    return 0;
}

// Kill the currently running thread.
void kill_current_thread(void) {
    raise_swi(SWI_CODE_DISABLE_IRQ);

    current_thread->status = not_existing;

    raise_swi(SWI_CODE_ENABLE_IRQ);

    for (;;);
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

void kill(unsigned int id) {
    struct tcb *t = find_tcb_by_id(id);
    if (t == 0) return;

    // We ignore if the thread is currently running. It will not be stopped immediately, but will not be scheduled
    // again.
    t->status = not_existing;
}

struct tcb *scheduler(void *pc, unsigned int registers[15]) {
    if (current_thread->status != not_existing) {
        // save context
        memcpy(current_thread->registers, registers, 15 * 4);
        asm("MRS %0, SPSR" : "=r" (current_thread->cpsr));
        current_thread->pc = pc;
        current_thread->status = ready;
    }

    // select next thread
    // round-robin, but higher priority (smaller number) always takes precedence
    struct tcb *next_tcb = 0;
    unsigned int next_thread_idx = 0;
    for (int i = 0; i < MAX_THREAD_COUNT; i++) {
        int idx = (current_thread_idx + i + 1) % MAX_THREAD_COUNT;

        struct tcb *t = &TCBS[idx];
        if (t->status == ready && (next_tcb == 0 || t->priority > next_tcb->priority)) {
            next_tcb = t;
            next_thread_idx = idx;
        }
    }

    if (next_tcb == 0) {
        return 0;
    }

    current_thread = next_tcb;
    current_thread_idx = next_thread_idx;

    return current_thread;
}

void scheduler_init(void) {
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        TCBS[i].sp_default = (void *) THREAD_STACK_START - (i * THREAD_STACK_SIZE);
    }
    current_thread = &TCBS[0];

    // create idle thread
    unsigned int idle_thread_id = fork(thread_nop);
    find_tcb_by_id(idle_thread_id)->priority = idle;
}

void print_tcb(struct tcb *t) {
    printf("tcb{\n"
           "  id = %d,\n"
           "  sp_default = %x,\n"
           "  status = %d,\n"
           "  pc = %x,\n"
           "  priority = %d,\n"
           "  registers = [\n", t->id, t->sp_default, t->status, t->pc, t->priority);
    for (int i = 0; i < 15; ++i) {
        printf("    %x,\n", t->registers[i]);
    }
    printf("  ]\n"
           "}\n");
}