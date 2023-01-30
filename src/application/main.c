#include <stdio.h>
#include <string.h>

#include "../drivers/dbgu.h"
#include "../interrupts/interrupts.h"

void handle_command(const char* cmd) {
    if (strcmp(cmd, "ping") == 0) {
        printf("pong\r\n");
    } else if (strcmp(cmd, "except swi") == 0) {
        cause_software_interrupt();
    } else if (strcmp(cmd, "except dabt") == 0) {
        cause_data_abort();
    } else if (strcmp(cmd, "except und") == 0) {
        cause_undefined_instruction();
    } else if (strcmp(cmd, "demo threads") == 0) {
        demo_threads();
    } else if (strcmp(cmd, "demo interrupts") == 0) {
        demo_interrupts();
    } else if (strcmp(cmd, "demo mmu") == 0) {
        demo_mmu();
    } else if (strcmp(cmd, "help") == 0) {
        printf("Commands:\n"
               "  demo <interrupts|threads|mmu>\n"
               "  except <swi|dabt|und>\n"
               "  help\n"
               "  ping\n"
               "\n"
               );
    } else {
        printf("Unrecognized command: \"%s\"\r\n", cmd);
    }
}

/**
 * This is the entry point for the application code. Initialization of the system is complete and this is now the main
 * thread.
 */
void main() {
    printf("\033[2J\033[H");
    printf("System initialized - switched to USR mode.\n");
    printf("If you don't know what to do, try \"help\".\n\n");

    char buf[128];
    int ibuf = 0;
    int escape_seq_remaining = 0; // number of chars remaining in escape sequence
    for (;;) {
        if (ibuf == 0){
            printf("\r$ ");
        }
        char rx = dbgu_receive();

        if (escape_seq_remaining > 0) {
            escape_seq_remaining--;
            continue;
        }

        // Escape codes for up, down, right, left: 'A', 'B', 'C', 'D'
        if (rx == '\n' || rx == '\r') {
            printf("\r\n", rx);
            handle_command(buf);
            ibuf = 0;

            for (int i = 0; i < 128; ++i) buf[i] = 0;

            continue;
        } else if (rx == '\b' || rx == 0x7f) {
            if (ibuf == 0) continue;

            buf[ibuf--] = 0;
            printf("\b \b");
            continue;
        } else if (rx == '\033') {
            escape_seq_remaining = 2;
            continue;
        }

        if (ibuf >= 127) {
            ibuf = 0;
            printf("\r\nCommand too long (>127 chars).\r\n");
        } else {
            buf[ibuf++] = rx;
            printf("%c", rx);
        }
    }
}
