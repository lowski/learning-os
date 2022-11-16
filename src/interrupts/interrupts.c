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

__attribute__((naked, section(".ivt")))
void ivt() {
    asm ("nop");
    asm ("b abt_handler");
    asm ("b abt_handler");
    asm ("b abt_handler");
    asm ("b abt_handler");
    asm ("nop");
    asm ("b abt_handler");
    asm ("b abt_handler");
}

__attribute__((naked, section(".handlers")))
void abt_handler() {
    printf("Fuck, there was an interrupt :/\r\n");
}