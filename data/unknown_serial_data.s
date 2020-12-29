	.section .rodata

	.align 2
gUnknown_089A3470:: @ 89A3470
	.incbin "data/unknown_serial_data.bin"

    .align 2
gFont1Chinese::
    //.incbin "graphics/fonts/font1_ch.bin"
    .incbin "graphics/fonts/fr1.bin",0,0x6BDC0
    .align 2
gFont0Chinese::
    .incbin "graphics/fonts/font0_ch.bin"
    //.incbin "graphics/fonts/fr1.bin",0x80000

