#include "ringbuffer.h"

struct RingBuffer RingBuffer_create(unsigned int size) {
    unsigned int buffer[size];
    struct RingBuffer rb;
    rb.buffer = buffer;
    rb.size = size;
    rb.head = 0;
    rb.length = 0;
    return rb;
}

void RingBuffer_push(struct RingBuffer *rb, unsigned int item) {
    if (rb->length == rb->size) {
        rb->head++;
        rb->length--;
    }
    unsigned int i = rb->head + rb->length;
    if (i > rb->size) {
        i -= rb->size; // no modulo to circumvent compiler weirdness
    }
    rb->buffer[i] = item;
    rb->length++;
}

unsigned int RingBuffer_pop(struct RingBuffer *rb) {
    if (rb->length == 0) {
        return 0;
    }
    unsigned int res = rb->buffer[rb->head];
    rb->length--;
    rb->head++;
    if (rb->head >= rb->size) {
        rb->head -= rb->size;
    }
    return res;
}

unsigned int RingBuffer_empty(struct RingBuffer *rb) {
    return rb->length == 0;
}