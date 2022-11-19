//
// Created by Leonard von Lojewski on 15.11.22.
//

#include "interrupts.h"
#include "../serial_interface/serial.h"

#define INVALID_MEMORY 0x9000000
static volatile char* invalid_memory = (char*)INVALID_MEMORY;

void cause_data_abort() {
    *invalid_memory = 1;
}

__attribute__((section(".handlers")))
void handler_fiq() {
    printf("There was a fiq interrupt!\r\n");
}

__attribute__((section(".handlers")))
void handler_irq() {
    printf("There was an irq interrupt!\r\n");
}

__attribute__((section(".handlers")))
void handler_pfabt() {
    printf("There was a prefetch abt interrupt!\r\n");
}

__attribute__((section(".handlers")))
void handler_dabt() {
    printf("There was a data abt interrupt!\r\n");
}

__attribute__((section(".handlers")))
void handler_und() {
    printf("There was an und interrupt!\r\n");
}

__attribute__((section(".handlers")))
void handler_swi() {
    printf("There was a swi interrupt!\r\n");
}