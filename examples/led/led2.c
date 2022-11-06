#define PIOB 0xfffff600
#define YELLOW_LED (1 << 27)

#define PIO_PER  (0x00 / 4)
#define PIO_OER  (0x10 / 4)
#define PIO_SODR (0x30 / 4)

static volatile unsigned int * const piob = (unsigned int *)PIOB;

void yellow_on(void)
{
	/* Initialisieren */
	piob[PIO_PER] = YELLOW_LED;
	piob[PIO_OER] = YELLOW_LED;

	/* Anschalten */
	piob[PIO_SODR] = YELLOW_LED;
}
