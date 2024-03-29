#ifndef LEARNING_OS_THREAD_H
#define LEARNING_OS_THREAD_H

#define MAX_THREAD_COUNT 16

void kill(unsigned int tid);
unsigned int clone(void *pc);

struct signal {
    unsigned int blocked_tid;
    char unblocked;
};
void signal(struct signal *s);
void wait(struct signal *s);
void sleep(unsigned int ms);

void threading_init();

#endif //LEARNING_OS_THREAD_H
