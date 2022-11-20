#ifndef LEARNING_OS_SERIAL_H
#define LEARNING_OS_SERIAL_H

void enable_dbgu(void);
void printf(const char*, ...);

unsigned int receive_byte(void);

#endif //LEARNING_OS_SERIAL_H
