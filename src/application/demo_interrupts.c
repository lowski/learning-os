#include <stdio.h>
#include "../drivers/dbgu.h"

unsigned int interrupt_demo_mode = 0;

void demo_interrupts() {
    interrupt_demo_mode = 1;

    for (;;) {
        unsigned char c = dbgu_receive();

        for (int i = 0; i < 200; ++i) {
            printf("%c", c);

            for (int j = 0; j < 1000000; ++j) asm("NOP");
        }
    }
}