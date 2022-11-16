ENTRY(_start)
SECTIONS
{
	. = 0x00200000;
	.ivt : { *(.ivt) }

	. = 0x002A0000;
    .handlers : { *(.handlers) }

	. = 0x20000000;
	.init : { *(.init) }
	.text : { *(.text) }
}

