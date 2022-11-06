#include "str.h"

int strlen(const char* str) {
    int i = 0;
    while (str[i] != 0) {
        i++;
    }
    return i;
}