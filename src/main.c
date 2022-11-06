#include "serial_interface/serial.h"

// main entry point
int main() {
    enable_dbgu();
    int i = 128;
    printf("test %d \r\n", 0x41);
    printf("test %s\r\n", "hi");
    printf("test %x\r\n", i);
    printf("test %p\r\n", &i);
//    transmit_string("abds\r\n");
//    transmit_byte(0x0d);
//    transmit_byte(0x0c);

    return 0;
}