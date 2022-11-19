ENTRY(_start)
SECTIONS
{
    /* IVT gets 256 bytes (0xff) */
	. = 0x00200000;
	.ivt : { *(.ivt) }

	. = 0x00200100;
    .handlers : { *(.handlers) }

    /* stacks for special modes get 0x002fffff downwards */

	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }
}

