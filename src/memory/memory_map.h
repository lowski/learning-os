#ifndef LEARNING_OS_MEMORY_MAP_H
#define LEARNING_OS_MEMORY_MAP_H


// DEBUG UNIT

// DBGU memory map -> Table 26-2
struct dbgu {
    unsigned int control; // mode; WO
    unsigned int mode; // mode; RW
    unsigned int interrupt_enable; // interrupt enable; WO
    unsigned int interrupt_disable; // interrupt disable; WO
    unsigned int interrupt_mask; // interrupt mask; RO

    // bitfield from 26.5.6
    struct {
        unsigned int rxrdy: 1; // rx ready
        unsigned int txrdy: 1; // tx ready
        unsigned int unused0: 1;
        unsigned int endrx: 1; // end of rx transfer
        unsigned int endtx: 1; // end of tx transfer
        unsigned int ovre: 1; // overrun error
        unsigned int frame: 1; // frame error
        unsigned int pare: 1; // parity error

        unsigned int unused1: 1;
        unsigned int txempty: 1; // transmitter empty
        unsigned int unused2: 1;
        unsigned int txbufe: 1; // tx buffer empty
        unsigned int rxbuff: 1; // rx buffer full
        unsigned int unused3: 3;

        unsigned char unused4;
        unsigned int unused5: 6;
        unsigned int commtx: 1; // debug communication channel write status
        unsigned int commrx: 1; // debug communication channel read status
    } status;

    unsigned int receive_holding; // receive holding; RO
    unsigned int transmit_holding; // transmit holding; WO
    unsigned int baud_rate_generator; // baud rate generator; RW
    unsigned int unused0[6];
    unsigned int chip_id; // chip id; RO
    unsigned int chip_id_extension; // extension; RO

    // some reserved space is left out
};

#define MASK_DBGU_IRQ_RXRDY 1
#define MASK_DBGU_IRQ_TXRDY 1 << 1
#define MASK_DBGU_IRQ_ENDRX 1 << 3 // end of rx
#define MASK_DBGU_IRQ_ENDTX 1 << 4 // end of tx
#define MASK_DBGU_IRQ_OVRE 1 << 5 // overrun error
#define MASK_DBGU_IRQ_FRAME 1 << 6 // framing error
#define MASK_DBGU_IRQ_PARE 1 << 7 // parity error
#define MASK_DBGU_IRQ_TXEMPTY 1 << 9 // tx empty
#define MASK_DBGU_IRQ_TXBUFE 1 << 11 // tx buffer empty
#define MASK_DBGU_IRQ_RXBUFF 1 << 12 // rx buffer full
#define MASK_DBGU_IRQ_COMMTX 1 << 30
#define MASK_DBGU_IRQ_COMMRX 1 << 31

#define ADDR_DBGU 0xFFFFF200 // Figure 8-1
static volatile struct dbgu * const dbgu = (struct dbgu *)ADDR_DBGU;

#endif //LEARNING_OS_MEMORY_MAP_H
