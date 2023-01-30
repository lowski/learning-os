#define INVALID_MEMORY 0x9000000

void cause_data_abort() {
    *(volatile char*)INVALID_MEMORY = 1;
}

void cause_software_interrupt() {
    asm("SWI 0");
}

void cause_undefined_instruction() {
    asm("LDMFD sp!,{pc}^");
}