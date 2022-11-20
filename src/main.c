#include "serial_interface/serial.h"
#include "interrupts/interrupts.h"
#include "memory/memory.h"

// main entry point
int main() {
    enable_dbgu();
    init_memory();
    printf("\033[2J\033[H");
    printf("System initialized - switching to USR mode...\n");
    asm("MSR CPSR_c, %0" :: "r" (MODE_USR));

    printf("Current mode: %b\r\n", get_current_mode());
    cause_data_abort();
    printf("Current mode: %b\r\n", get_current_mode());
    cause_software_interrupt();
    printf("Current mode: %b\r\n", get_current_mode());
    cause_undefined_instruction();
    printf("Current mode: %b\r\n", get_current_mode());

    printf("Done.\r\n");

    for (;;);

    // we cannot return from this function, as the .init section is now shadowed by the ivt, so if we try to return
    // to the _start() function, we will land in an interrupt handler.
}
