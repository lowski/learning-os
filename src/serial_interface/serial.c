#include "serial.h"
#include "../stdlib/str.h"
#include "stdarg.h"

#define DBGU 0xFFFFF200 // Figure 8-1

struct dbgu_mm_cr {
    //     unsigned int cr; // control; WO
    // bitfield from 26.5.1
    unsigned int cr_unused0: 2;
    unsigned int cr_rstrx: 1;
    unsigned int cr_rsttx: 1;
    unsigned int cr_rxen: 1;
    unsigned int cr_rxdis: 1;
    unsigned int cr_txen: 1;
    unsigned int cr_txdis: 1;

    unsigned int cr_rststa: 1;
    unsigned int cr_unused1: 7;
    unsigned short cr_unused2;
};

// DBGU memory map -> Table 26-2
struct dbgu_mm {
    unsigned int cr; // mode; WO
    unsigned int mr; // mode; RW
    unsigned int ier; // interrupt enable; WO
    unsigned int idr; // interrupt disable; WO
    unsigned int imr; // interrupt mask; RO

//    unsigned int sr; // status register; RO
//  bitfield from 26.5.6
    unsigned int sr_rxrdy: 1; // rx ready
    unsigned int sr_txrdy: 1; // tx ready
    unsigned int sr_unused0: 1;
    unsigned int sr_endrx: 1; // end of rx transfer
    unsigned int sr_endtx: 1; // end of tx transfer
    unsigned int sr_ovre: 1; // overrun error
    unsigned int sr_frame: 1; // frame error
    unsigned int sr_pare: 1; // parity error

    unsigned int sr_unused1: 1;
    unsigned int sr_txempty: 1; // transmitter empty
    unsigned int sr_unused2: 1;
    unsigned int sr_txbufe: 1; // tx buffer empty
    unsigned int sr_rxbuff: 1; // rx buffer full
    unsigned int sr_unused3: 3;

    unsigned char sr_unused4;
    unsigned int sr_unused5: 6;
    unsigned int sr_commtx: 1; // debug communication channel write status
    unsigned int sr_commrx: 1; // debug communication channel read status

    unsigned int rhr; // receive holding; RO
    unsigned int thr; // transmit holding; WO
    unsigned int brgr; // baud rate generator; RW
    unsigned int unused0[6];
    unsigned int cidr; // chip id; RO
    unsigned int exid; // extension; RO

    // some reserved space is left out
};

static volatile struct dbgu_mm * const dbgu = (struct dbgu_mm *)DBGU;

// enable DBGU transmitter (26.4.3)
void enable_dbgu(void) {
    dbgu->cr = (1 << 6) | (1 << 4); // for offset see dbgu_mm_cr
}

void transmit_byte(unsigned char byte) {
    while (dbgu->sr_txrdy != 1);

    dbgu->thr = byte;
}

unsigned int receive_byte() {
    while (dbgu->sr_rxrdy != 1);

    return dbgu->rhr;
}

void transmit_string(const char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; ++i) {
        transmit_byte(str[i]);
    }
    while (dbgu->sr_txempty != 1);
}

const char alphabet[62] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int ilen(int value, int base) {
    int i = 0;
    unsigned int val = value;
    unsigned int rest;
    while (val > 0) {
        rest = val % base;
        ++i;
        val = (val - rest) / base;
    }
    return i;
}

static char* itoa(int value, char* str, int base) {
    int i = 0;
    int val = value;
    while (val > 0) {
        int rest = val % base;
        str[i] = alphabet[rest];
        ++i;
        val = (val - rest) / base;
    }
    return str;
}

__attribute__((format(printf, 1, 2)))
void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int len = strlen(fmt);
    char escape = 0;
    for (int i = 0; i < len; ++i) {
        const char c = fmt[i];
        if (escape) {
            // no handling if the type is wrong.
            if (c == 'c') {
                    transmit_byte(va_arg(args, unsigned int));
            } else if (c == 's') {
                    transmit_string(va_arg(args, const char*));
            } else if (c == 'x') {
                    unsigned int arg = va_arg(args, unsigned int);
                    int converted_length = ilen(arg, 16);
                    char converted[converted_length + 1];
                    itoa(arg, converted, 16);
                    converted[converted_length] = '\0';
                    transmit_string(converted);
            } else if (c == 'p') {
                    void *arg = va_arg(args, void *);
                    int converted_length = ilen((unsigned int) arg, 16);
                    char converted[converted_length + 1];
                    itoa((unsigned int) arg, converted, 16);
                    converted[converted_length] = '\0';
                    transmit_string(converted);
            } else {
                    transmit_byte('%');
                    transmit_byte(c);
            }

            escape = 0;
            continue;
        }

        if (c == '%') {
            escape = 1;
            continue;
        }

        transmit_byte(c);
    }

    va_end(args);

    while (dbgu->sr_txempty != 1);
}
