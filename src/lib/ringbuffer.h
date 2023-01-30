#ifndef LEARNING_OS_RINGBUFFER_H
#define LEARNING_OS_RINGBUFFER_H

struct RingBuffer {
    unsigned int *buffer;
    unsigned int size;
    unsigned int head;
    unsigned int length;
};

struct RingBuffer RingBuffer_create(unsigned int size);
void RingBuffer_push(struct RingBuffer* rb, unsigned int item);
unsigned int RingBuffer_pop(struct RingBuffer *rb);
unsigned int RingBuffer_empty(struct RingBuffer *rb);

#endif //LEARNING_OS_RINGBUFFER_H
