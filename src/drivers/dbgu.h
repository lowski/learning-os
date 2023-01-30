#ifndef LEARNING_OS_DBGU_H
#define LEARNING_OS_DBGU_H

#include <stdint.h>

void dbgu_init();
void dbgu_transmit(unsigned char);
unsigned char dbgu_receive();
void dbgu_handle_irq();

/**
 * Check whether there is an incoming character waiting in the debug unit.
 * @return 0 or 1
 */
bit_t dbgu_status_rx_ready();

/**
 * Check whether the tx buffer is empty, meaning the last character in the buffer was sent.
 * @return 0 or 1
 */
bit_t dbgu_status_tx_empty();

#endif //LEARNING_OS_DBGU_H
