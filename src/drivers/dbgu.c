#include "dbgu.h"
#include "../stdlib/stdio.h"
#include "../stdlib/threading.h"
#include "../interrupts/scheduling.h"
#include "../stdlib/datatypes.h"

unsigned char receive_buffer = 0;
unsigned char transmit_buffer = 0;

struct signal receive_signal = {};
struct signal tx_request_signal = {};
struct signal tx_done_signal = {};

void dbgu_transmit_blocking(unsigned char byte) {
    while (dbgu->status.txrdy != 1);

    dbgu->transmit_holding = byte;
}

void dbgu_transmit_thread() {
    for (;;) {
        wait(&tx_request_signal);

        if (transmit_buffer == 0) {
            continue;
        }
        dbgu_transmit_blocking(transmit_buffer);

        signal(&tx_done_signal);
    }
}

void dbgu_init(void) {
    // enable receive and transmit
    dbgu->control = (1 << 6) | (1 << 4);
    dbgu->interrupt_enable = dbgu->interrupt_mask | DBGU_MASK_IRQ_RXRDY;

    unsigned int tid = clone(dbgu_transmit_thread);
    find_tcb_by_id(tid)->priority = high;
}

void dbgu_transmit(unsigned char byte) {
    dbgu_transmit_blocking(byte);
//    transmit_buffer = byte;
//    dbgu_transmit_blocking('x');
//    signal(&tx_request_signal);
//    wait(&tx_done_signal);
}

unsigned char dbgu_receive() {
    // receiver overrun is being ignored.
    if (receive_buffer == 0) {
        wait(&receive_signal);
    }

    unsigned char res = receive_buffer;
    receive_buffer = 0;
    return res;
}

void dbgu_handle_irq() {
    if (dbgu->status.rxrdy) {
        // if the previous character has not been accessed yet, it's gone now.
        receive_buffer = dbgu->receive_holding;
        signal(&receive_signal);
    }
}
