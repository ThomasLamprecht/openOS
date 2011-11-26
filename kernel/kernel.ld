/*  starting execution on label _start */
ENTRY(_start)

/*
 * Defines in which order the sections will be written in the binary
 */
SECTIONS
{
    /*
     * . describes the actual position in the File. I will load the kernel on 1 MB so there will be our first section
     */
    . = 0x100000;

	/* define a symbol for kernel start (needed for PMM)) */
    kernel_start = .;
    /*
     * The multiboot-header must be in the first 8kB so lets define him as first section
	 * after him the standar sections comes
     */
    .text : {
        *(multiboot)
        *(.text)
    }
    .data ALIGN(4096) : {
        *(.data)
    }
    .rodata ALIGN(4096) : {
        *(.rodata)
    }
    .bss ALIGN(4096) : {
        *(.bss)
    }
    /* define a symbol for kernel end and lets ceils it to 4k */
    . = ALIGN(4096);
    kernel_end = .;
}
