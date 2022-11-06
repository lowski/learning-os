#ifndef LEARNING_OS_SERIAL_H
#define LEARNING_OS_SERIAL_H

void enable_dbgu(void);
void transmit_byte(unsigned char);
void transmit_string(const char*);
void printf(const char*, ...);

#endif //LEARNING_OS_SERIAL_H
