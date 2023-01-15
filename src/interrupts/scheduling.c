#include "scheduling.h"
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

void thread_nop() {
    for (;;) asm("NOP");
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
    t->sp = t->sp_default;
    t->status = ready;
    t->pc = pc;

    return t->id;
}

void kill(unsigned int id) {
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        if (TCBS[i].id == id) {
            TCBS[i].status = not_existing;
            break;
        }
    }
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
    struct tcb *next_tcb = 0;
    for (int i = 0; i <= MAX_THREAD_COUNT; i++) {
        current_thread_idx = (current_thread_idx + 1) % MAX_THREAD_COUNT;
        if (TCBS[current_thread_idx].status == ready) {
            next_tcb = &TCBS[current_thread_idx];
            break;
        }
    }

    if (next_tcb == 0) {
        return 0;
    }

    current_thread = next_tcb;

    return current_thread;
}

void scheduler_init(void) {
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        TCBS[i].sp_default = (void *) THREAD_STACK_START - (i * THREAD_STACK_SIZE);
    }
    current_thread = &TCBS[0];

    // create idle thread
//    fork(thread_nop);
}

void print_tcb(struct tcb *t) {
    printf("tcb{\n"
           "  id = %d,\n"
           "  sp = %x,\n"
           "  sp_default = %x,\n"
           "  status = %d,\n"
           "  pc = %x,\n"
           "  registers = [\n", t->id, t->sp, t->sp_default, t->status, t->pc);
    for (int i = 0; i < 15; ++i) {
        printf("    %x,\n", t->registers[i]);
    }
    printf("  ]\n"
           "}\n");
}