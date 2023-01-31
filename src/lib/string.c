#include <string.h>

unsigned int strlen(const char* str) {
    int i = 0;
    while (str[i] != 0) {
        i++;
    }
    return i;
}

unsigned int strcmp(const char *str1, const char *str2) {
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

void *memcpy(void *dest, void *src, int count) {
    for (int i = 0; i < count; ++i) {
        ((char *)dest)[i] = ((char *)src)[i];
    }
    return dest;
}

void *memset(void *str, int c, int n) {
    for (int i = 0; i < n; ++i) {
        ((char *) str)[i] = (char) c;
    }
    return str;
}

char *strcpy(char *dest, const char *src) {
    return (char *)memcpy(dest, (void *) src, (int) strlen(src) + 1);
}