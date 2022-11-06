#include "serial_interface/serial.h"

// main entry point
int main() {
    enable_dbgu();
    int i = 128;
    printf("test %c \r\n", 0x41);
    printf("test %s\r\n", "hi");
    printf("test %x\r\n", i);
    printf("test %p\r\n", &i);

    char rcv;
    while(1) {
        rcv = (char) receive_byte();
        printf("You have typed: %c\r\n", rcv);
    }

    return 0;
}