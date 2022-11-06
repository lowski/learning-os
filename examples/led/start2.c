#include "led.h"

__attribute__((naked, section(".init")))
void _start(void)
{
        yellow_on();
        for(;;);
}

