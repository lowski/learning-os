#include <thread.h>

#include "dbgu.h"
#include "../interrupts/scheduling.h"
#include "memory.h"

#define DBGU_MASK_IRQ_RXRDY 1
#define DBGU_MASK_IRQ_TXRDY 1 << 1
#define DBGU_MASK_IRQ_ENDRX 1 << 3 // end of rx
#define DBGU_MASK_IRQ_ENDTX 1 << 4 // end of tx
#define DBGU_MASK_IRQ_OVRE 1 << 5 // overrun error
#define DBGU_MASK_IRQ_FRAME 1 << 6 // framing error
#define DBGU_MASK_IRQ_PARE 1 << 7 // parity error
#define DBGU_MASK_IRQ_TXEMPTY 1 << 9 // tx empty
#define DBGU_MASK_IRQ_TXBUFE 1 << 11 // tx buffer empty
#define DBGU_MASK_IRQ_RXBUFF 1 << 12 // rx buffer full
#define DBGU_MASK_IRQ_COMMTX 1 << 30
#define DBGU_MASK_IRQ_COMMRX 1 << 31

// DBGU memory map -> Table 26-2
struct dbgu {
    uint32_t control; // mode; WO
    uint32_t mode; // mode; RW
    uint32_t interrupt_enable; // interrupt enable; WO
    uint32_t interrupt_disable; // interrupt disable; WO
    uint32_t interrupt_mask; // interrupt mask; RO

    // bitfield from 26.5.6
    struct {
        bit_t rxrdy: 1; // rx ready
        bit_t txrdy: 1; // tx ready
        bit_t: 1;
        bit_t endrx: 1; // end of rx transfer
        bit_t endtx: 1; // end of tx transfer
        bit_t ovre: 1; // overrun error
        bit_t frame: 1; // frame error
        bit_t pare: 1; // parity error
        bit_t: 1;
        uint32_t txempty: 1; // transmitter empty; for some reason this cannot be uint8, so it's not...
        bit_t: 1;
        bit_t txbufe: 1; // tx buffer empty
        bit_t rxbuff: 1; // rx buffer full
        uint_t: 17;
        bit_t commtx: 1; // debug communication channel write status
        bit_t commrx: 1; // debug communication channel read status
    } status;

    uint32_t receive_holding; // receive holding; RO
    uint32_t transmit_holding; // transmit holding; WO
    uint32_t baud_rate_generator; // baud rate generator; RW
    uint32_t unused0[6];
    uint32_t chip_id; // chip id; RO
    uint32_t chip_id_extension; // extension; RO

    // some reserved space is left out
};

static volatile struct dbgu *const dbgu = (struct dbgu *) MEM_ADDR_PERIPHERY_DBGU;

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

FUNC_PRIVILEGED
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

FUNC_PRIVILEGED
void dbgu_handle_irq() {
    if (dbgu->status.rxrdy) {
        // if the previous character has not been accessed yet, it's gone now.
        receive_buffer = dbgu->receive_holding;
        signal(&receive_signal);
    }
}

bit_t dbgu_status_rx_ready() {
    return dbgu->status.rxrdy;
}

bit_t dbgu_status_tx_empty() {
    return dbgu->status.txempty;
}
