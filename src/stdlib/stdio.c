#include "stdarg.h"

#include "../drivers/dbgu.h"
#include "stdio.h"
#include "str.h"


void transmit_string(const char* str) {
    int len = strlen(str);
    for (int i = 0; i < len; ++i) {
        dbgu_transmit(str[i]);
    }
    while (dbgu->status.txempty != 1);
}

const char alphabet[62] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

inline int ilen(int value, int base) {
    if (value == 0) {
        return 1;
    }
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

inline char* itoa(int value, char* str, int base) {
    if (value == 0) {
        str[0] = '0';
        return str;
    }
    int i = ilen(value, base) - 1;
    int val = value;
    while (i >= 0) {
        int rest = val % base;
        str[i] = alphabet[rest];
        val = (val - rest) / base;
        --i;
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
                dbgu_transmit(va_arg(args, unsigned int));
            } else if (c == 's') {
                transmit_string(va_arg(args, const char*));
            } else if (c == 'x' || c == 'p') {
                unsigned int arg = va_arg(args, unsigned int);
                int converted_length = ilen(arg, 16);
                char converted[converted_length + 3];
                itoa(arg, converted + 2, 16);
                converted[0] = '0';
                converted[1] = 'x';
                converted[converted_length + 2] = '\0';
                transmit_string(converted);
            } else if (c == 'b') {
                unsigned int arg = va_arg(args, unsigned int);
                int converted_length = ilen(arg, 2);
                char converted[converted_length + 3];
                itoa(arg, converted + 2, 2);
                converted[0] = '0';
                converted[1] = 'b';
                converted[converted_length + 2] = '\0';
                transmit_string(converted);
            } else {
                dbgu_transmit('%');
                dbgu_transmit(c);
            }

            escape = 0;
            continue;
        }

        if (c == '%') {
            escape = 1;
            continue;
        }

        dbgu_transmit(c);
    }

    va_end(args);

    while (dbgu->status.txempty != 1);
}
