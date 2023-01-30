#include <stdio.h>
#include "../../include/thread.h"

void thread_a() {
    for (int i = 0; i < 5; i++) {
        printf("Thread A (%d)\n", i);
        for (int j = 0; j < 50000000; ++j) asm("NOP");
    }
    sleep(5000);
    printf("Thread A is awake.\n");
    for (int i = 0; i < 5; i++) {
        printf("Thread A (%d)\n", i);
        for (int j = 0; j < 50000000; ++j) asm("NOP");
    }
}


void thread_b() {
    for (int i = 0; i < 30; i++) {
        printf("Thread B (%d)\n", i);
        for (int j = 0; j < 50000000; ++j) asm("NOP");
    }
}

void demo_threads() {
    clone(thread_a);
    clone(thread_b);
}