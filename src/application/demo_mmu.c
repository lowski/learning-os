#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>

void demo_mmu() {
    printf("Accessing null pointer...\n\n");
    asm("MOV r0, #0");
    asm("LDR r0, [r0]");

    printf("\nReading demo_mmu function (kernel code)...\n");
    printf("first instruction of demo_mmu function: %x\n", *(uint32_t *)demo_mmu);

    printf("\nThere is no kernel data, as we unfortunately need everything in the user space (see memory.c:184).\n");

    printf("\nWriting demo_mmu code...\n");
    *(uint32_t *)demo_mmu = 0xdeadbeef;

    printf("\nWe can't produce a stack overflow, as the stacks are all after one-another and currently not separated by any memory that could produce a fault.\n");

    printf("\nReading non-existent address (0xefff0000)...\n");
    printf("Value: %x\n", *(uint32_t *)0xefff0000);

    printf("\nThe IVT is mapped through the translation table, which demonstrates non 1:1 mapping (see memory.c:171)\n");
}
char *ptr = NULL;

void alt_thread() {
    printf("[alt_thread] Accessing ptr...\n");
    printf("[alt_thread] Value at *ptr: %x\n", *ptr);
}

void demo_malloc() {
    printf("Allocating 16 bytes and setting 15 to '_'...\n");
    ptr = (char *) malloc(16);
    memset(ptr, '_', 15);
    printf("Address: %x, value: %s\n", ptr, ptr);
    printf("Writing to [16]...\n");
    ptr[16] = 'c';

    printf("\nSpawning second thread...\n");
    clone(alt_thread);
    sleep(1000);
    free(ptr);
}