ENTRY(start)
SECTIONS
{
    /* IVT gets 256 bytes (0xff) */
	. = 0x00200000;
	.ivt : { *(.ivt) }

	. = 0x00200100;
    .handlers : { *(.handlers) }


	. = 0x20000000;
	.init : { *(.init) }
	.text_privileged : { *(.text_privileged) }

	. = 0x20100000;
	.text : { *(.text) }
	. = 0x20200000;
	.data : { *(.data) }

    /* RAM (0x2000_0000 - 0x2400_0000) */
    /* 0x2110_0000: master page table */
    /* 0x2120_0000: special mode stacks (0x200 bytes each) */
    /* 0x2130_0000: TCBs (0x80 bytes each x 16) */
    /* 0x2140_0000: thread stacks (0x200 bytes each) */
    /* 0x2200_0000: heap */
}

