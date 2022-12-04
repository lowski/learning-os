#include "dbgu.h"

void dbgu_init(void) {
    // enable receive and transmit
    dbgu->control = (1 << 6) | (1 << 4);
}

void dbgu_transmit(unsigned char byte) {
    while (dbgu->status.txrdy != 1);

    dbgu->transmit_holding = byte;
}

unsigned char dbgu_receive() {
    // receiver overrun is being ignored.
    while (dbgu->status.rxrdy != 1);

    return dbgu->receive_holding;
}
