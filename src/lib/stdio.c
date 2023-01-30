#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "../drivers/dbgu.h"

unsigned int transmit_string(const char* str) {
    unsigned int len = strlen(str);
    for (unsigned int i = 0; i < len; ++i) {
        putchar(str[i]);
    }
    while (dbgu_status_tx_empty() != 1);
    return len;
}

const char alphabet[62] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

inline unsigned int ilen(unsigned int value, int base) {
    if (value == 0) {
        return 1;
    }
    unsigned int i = 0;
    unsigned int val = value;
    unsigned int rest;
    while (val > 0) {
        rest = val % base;
        ++i;
        val = (val - rest) / base;
    }
    return i;
}

inline char* itoa(unsigned int value, char* str, int base) {
    if (value == 0) {
        str[0] = '0';
        return str;
    }
    int i = ilen(value, base) - 1;
    unsigned int val = value;
    while (i >= 0) {
        int rest = val % base;
        str[i] = alphabet[rest];
        val = (val - rest) / base;
        --i;
    }
    return str;
}

__attribute__((format(printf, 1, 2)))
int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    int len = strlen(fmt);
    char escape = 0;
    unsigned int printed_chars = 0;
    for (int i = 0; i < len; ++i) {
        const char c = fmt[i];
        if (escape) {
            // no handling if the type is wrong.
            if (c == 'c') {
                dbgu_transmit(va_arg(args, unsigned int));
            } else if (c == 's') {
                printed_chars += transmit_string(va_arg(args, const char*));
            } else if (c == 'x' || c == 'p') {
                unsigned int arg = va_arg(args, unsigned int);
                unsigned int converted_length = ilen(arg, 16);
                char converted[converted_length + 3];
                itoa(arg, converted + 2, 16);
                converted[0] = '0';
                converted[1] = 'x';
                converted[converted_length + 2] = '\0';
                printed_chars += transmit_string(converted);
            } else if (c == 'd') {
                unsigned int arg = va_arg(args, unsigned int);
                unsigned int converted_length = ilen(arg, 10);
                char converted[converted_length + 1];
                itoa(arg, converted, 10);
                converted[converted_length] = '\0';
                printed_chars += transmit_string(converted);
            } else if (c == 'b') {
                unsigned int arg = va_arg(args, unsigned int);
                unsigned int converted_length = ilen(arg, 2);
                char converted[converted_length + 3];
                itoa(arg, converted + 2, 2);
                converted[0] = '0';
                converted[1] = 'b';
                converted[converted_length + 2] = '\0';
                printed_chars += transmit_string(converted);
            } else {
                putchar('%');
                putchar(c);
                printed_chars += 2;
            }

            escape = 0;
            continue;
        }

        if (c == '%') {
            escape = 1;
            continue;
        }

        putchar(c);
        printed_chars++;
    }

    va_end(args);

    while (dbgu_status_tx_empty() != 1);
    return (int) printed_chars;
}

int getchar() {
    return (int) dbgu_receive();
}

int putchar(int c) {
    unsigned char cast = (unsigned char) c;
    dbgu_transmit(cast);
    return cast;
}
