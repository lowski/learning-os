#include "str.h"

int strlen(const char* str) {
    int i = 0;
    while (str[i] != 0) {
        i++;
    }
    return i;
}

int strcmp(const char *str1, const char *str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if (len1 != len2) {
        if (len1 > len2) {
            return 1;
        } else {
            return -1;
        }
    }

    for (int i = 0; i < len1; ++i) {
        if (str1[i] == str2[i]) {
            continue;
        }

        if (len1 > len2) {
            return 1;
        } else {
            return -1;
        }
    }
    return 0;
}