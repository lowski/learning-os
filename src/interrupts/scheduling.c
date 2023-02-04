#include <string.h>
#include <stdio.h>
#include <thread.h>

#include "scheduling.h"
#include "../drivers/swi.h"
#include "../drivers/memory.h"

struct tcb *TCBS = (struct tcb *)MEM_ADDR_TCBS;
struct tcb *current_thread;

extern void sleeping_beauty();

void thread_nop() {
    for (;;) asm("NOP");
}

unsigned int get_current_thread_id() {
    return current_thread->id;
}

struct tcb *get_current_tcb() {
    return current_thread;
}

// Kill the currently running thread.
void kill_current_thread(void) {
    raise_swi(SWI_CODE_DISABLE_IRQ);

    current_thread->status = not_existing;

    raise_swi(SWI_CODE_ENABLE_IRQ);

    reschedule();
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

/**
 * Save the context of the current thread. If `tcb` is 0, this is a no-op.
 *
 * @param tcb the TCB of the thread to store the context to
 * @param pc the instruction pointer of the thread
 * @param registers list of the registers (r0 to r14, with r0 at [0] and r14 at [14])
 */
FUNC_PRIVILEGED
void save_context(struct tcb *tcb, void *pc, unsigned int registers[15]) {
    if (tcb->status != not_existing) {
        memcpy(tcb->registers, registers, 15 * 4);
        asm("MRS %0, SPSR" : "=r" (tcb->cpsr));
        tcb->pc = pc;
        if (tcb->status == running) {
            tcb->status = ready;
        }
    }
}

/**
 * Restore the context of the next thread: PC, registers, CPSR. If `next_tcb` is 0, this is a no-op.
 *
 * @param next_tcb the TCB of the next thread
 * @param registers the list of registers to where the context should be restored
 * @return the instruction pointer to return to (0 if next_tcb is 0)
 */
FUNC_PRIVILEGED
void *restore_context(struct tcb *next_tcb, unsigned int registers[15]) {
    void *pc = 0;
    if (next_tcb != 0) {
        pc = next_tcb->pc;
        asm("MSR SPSR, %0" :: "r" (next_tcb->cpsr));
        memcpy(registers, next_tcb->registers, 15*4);

        current_thread = next_tcb;
        current_thread->status = running;

        mem_swap_heap(next_tcb->id);
    }

    return pc;
}

FUNC_PRIVILEGED
struct tcb *select_next_thread() {
    // round-robin, but higher priority (smaller number) always takes precedence
    struct tcb *next_tcb = 0;
    for (int i = 0; i < MAX_THREAD_COUNT; i++) {
        int idx = (current_thread->idx + i + 1) % MAX_THREAD_COUNT;

        struct tcb *t = &TCBS[idx];
        if (t->status == ready && (next_tcb == 0 || t->priority > next_tcb->priority)) {
            next_tcb = t;
        }
    }
    return next_tcb;
}

FUNC_PRIVILEGED
void *scheduler(void *pc, unsigned int registers[15]) {
    save_context(current_thread, pc, registers);
    struct tcb *next_tcb = select_next_thread();
    void *new_pc = restore_context(next_tcb, registers);

    if (new_pc == 0) {
        return pc;
    }
    return new_pc;
}

FUNC_PRIVILEGED
void scheduler_init(void) {
    for (int i = 0; i < MAX_THREAD_COUNT; ++i) {
        TCBS[i].sp_default = (void *) MEM_ADDR_THREAD_STACKS + (1 + i * MEM_SIZE_THREAD_STACK);
        TCBS[i].idx = i;
    }
    current_thread = &TCBS[0];

    // create idle thread
    unsigned int idle_thread_id = clone(thread_nop);
    find_tcb_by_id(idle_thread_id)->priority = idle;

    clone(sleeping_beauty);
}

void print_tcb(struct tcb *t) {
    printf("tcb{\n"
           "  id = %d,\n"
           "  sp_default = %x,\n"
           "  status = %d,\n"
           "  pc = %x,\n"
           "  priority = %d,\n"
           "}\n", t->id, t->sp_default, t->status, t->pc, t->priority);
}

void reschedule() {
    raise_swi(SWI_CODE_RESCHEDULE);
}

void block(unsigned int tid, __attribute__((unused)) struct signal* s) {
    struct tcb *t = find_tcb_by_id(tid);
    if (t == 0) {
        return;
    }
    t->status = blocked;

    reschedule();
}


void unblock(unsigned int tid) {
    struct tcb *t = find_tcb_by_id(tid);
    if (t == 0) {
        return;
    }
    if (t->status == blocked) {
        t->status = ready;
    }
}