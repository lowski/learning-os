ENTRY(_start)
SECTIONS
{
    /* IVT gets 256 bytes (0xff) */
	. = 0x00200000;
	.ivt : { *(.ivt) }

	. = 0x00200100;
    .handlers : { *(.handlers) }


	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }


    /* RAM (0x2000_0000 - 0x2fff_ffff) */
    /* 0x2010_0000: master page table */
    /* 0x2020_0000: special mode stacks (0x200 bytes each) */
    /* 0x2030_0000: TCBs (0x80 bytes each x 16) */
    /* 0x2040_0000: thread stacks (0x200 bytes each) */
}

