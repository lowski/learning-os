#include "dbgu.h"
#include "../stdlib/stdio.h"

unsigned char receive_buffer = 0;

void dbgu_init(void) {
    // enable receive and transmit
    dbgu->control = (1 << 6) | (1 << 4);
    dbgu->interrupt_enable = dbgu->interrupt_mask | DBGU_MASK_IRQ_RXRDY;
}

void dbgu_transmit(unsigned char byte) {
    while (dbgu->status.txrdy != 1);

    dbgu->transmit_holding = byte;
}

unsigned char dbgu_receive() {
    // receiver overrun is being ignored.
    while (receive_buffer == 0) asm("NOP");

    unsigned char res = receive_buffer;
    receive_buffer = 0;
    return res;
}

void dbgu_handle_irq() {
    if (dbgu->status.rxrdy) {
        // if the previous character has not been accessed yet, it's gone now.
        receive_buffer = dbgu->receive_holding;
    }
}
