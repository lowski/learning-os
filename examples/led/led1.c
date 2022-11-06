#define PIOB 0xfffff600
#define YELLOW_LED (1 << 27)

#define PIO_PER 0x00
#define PIO_OER 0x10
#define PIO_SODR 0x30

static inline void write_u32(unsigned int addr, unsigned int val)
{
	*(volatile unsigned int *)addr = val;
}

void yellow_on(void)
{
	/* Initialisieren */
	write_u32(PIOB + PIO_PER, YELLOW_LED);
	write_u32(PIOB + PIO_OER, YELLOW_LED);

	/* Anschalten */
	write_u32(PIOB + PIO_SODR, YELLOW_LED);
}
