#ifndef LEARNING_OS_STDLIB_H
#define LEARNING_OS_STDLIB_H

#include "stdint.h"

ptr_t malloc(__attribute__((unused)) uint32_t size);
void free(__attribute__((unused)) ptr_t ptr);

#endif //LEARNING_OS_STDLIB_H
