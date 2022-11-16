#include "serial_interface/serial.h"
#include "interrupts/interrupts.h"
#include "memory/memory.h"

// main entry point
int main() {
    enable_dbgu();
    init_memory();
    printf("Booted.\r\n");

    cause_data_abort();

    printf("Done.\r\n");
    return 0;
}
