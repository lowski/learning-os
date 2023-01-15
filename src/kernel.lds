ENTRY(_start)
SECTIONS
{
    /* IVT gets 256 bytes (0xff) */
	. = 0x00200000;
	.ivt : { *(.ivt) }

	. = 0x00200100;
    .handlers : { *(.handlers) }

    /* thread control blocks get 0x2F0000 upwards */
    /* stacks for threads get 0x002FD400 to 0x2ff3ff */
    /* every thread gets 512B, so 16 threads are supported */
    /* stacks for special modes get 0x2ff400 to 0x2fffff */

	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }
}

