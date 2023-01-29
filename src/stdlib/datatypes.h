#ifndef LEARNING_OS_DATATYPES_H
#define LEARNING_OS_DATATYPES_H

typedef int int32_t;
typedef char int8_t;
typedef int int_t;

typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned int uint_t;
typedef unsigned char bit_t;

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

#endif //LEARNING_OS_DATATYPES_H
