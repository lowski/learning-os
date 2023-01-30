#include <stdint.h>
#include <stdio.h>

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