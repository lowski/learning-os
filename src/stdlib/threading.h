#ifndef LEARNING_OS_THREADING_H
#define LEARNING_OS_THREADING_H

#define MAX_THREAD_COUNT 16

void kill(unsigned int tid);
unsigned int fork(void *pc);

#endif //LEARNING_OS_THREADING_H
