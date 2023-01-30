#include <thread.h>
#include "drivers/memory.h"
#include "interrupts/scheduling.h"
#include "drivers/aic.h"
#include "drivers/system_timer.h"
#include "drivers/dbgu.h"

extern void main(void);

__attribute__((unused, section(".init")))
_Noreturn void start(void)
{
    init_memory();
    scheduler_init();
    aic_init();
    system_timer_init();
    dbgu_init();

    clone(main);
    reschedule();

    for (;;);
}
