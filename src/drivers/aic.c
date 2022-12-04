#include "aic.h"

void aic_init() {
    // enable interrupts on channel 1 (internal periphery).
    aic->interrupt_enable_command = 0b10;

    // set source 1 to positive-edge triggered.
    aic->source_mode[1] = 1 << 5;
}