#ifndef LEARNING_OS_THREADING_H
#define LEARNING_OS_THREADING_H

#define MAX_THREAD_COUNT 16

void kill(unsigned int tid);
unsigned int fork(void *pc);


struct signal {
    unsigned int blocked_tid;
};
void signal(struct signal *s);
void wait(struct signal *s);
void sleep(unsigned int ms);

void threading_init();

#endif //LEARNING_OS_THREADING_H
