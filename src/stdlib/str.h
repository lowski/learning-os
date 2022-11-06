//
// Created by Leonard von Lojewski on 06.11.22.
//

#ifndef LEARNING_OS_STR_H
#define LEARNING_OS_STR_H

int strlen(const char*);

static int ilen(int value, int base);
static char* itoa(int value, char* str, int base);

#endif //LEARNING_OS_STR_H
