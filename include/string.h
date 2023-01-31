#ifndef LEARNING_OS_STRING_H
#define LEARNING_OS_STRING_H

unsigned int strlen(const char *s);
unsigned int strcmp(const char *str1, const char *str2);
char *strcpy(char *dest, const char *src);
void *memcpy(void *dest, void *src, int count);
void *memset(void *str, int c, int n);

#endif //LEARNING_OS_STRING_H
