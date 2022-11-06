extern int main(void);

__attribute__((naked, section(".init")))
void _start(void)
{
        main();
        for(;;);
}

