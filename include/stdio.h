#ifndef LEARNING_OS_STDIO_H
#define LEARNING_OS_STDIO_H

int printf(const char*, ...);

/**
 * Get a character (unsigned char) from stdin.
 *
 * @return the character read as unsigned char cast to an int.
 */
int getchar(void);

/**
 * Write a character (unsigned char) to stdout.
 *
 * @param c the character to be written. Passed as its int promotion.
 * @return character written as unsigned char cast to an int.
 */
int putchar(int c);

#endif //LEARNING_OS_STDIO_H
