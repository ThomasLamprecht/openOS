.section multiboot
#define MB_MAGIC 0x1badb002
#define MB_FLAGS 0x0
#define MB_CHECKSUM -(MB_MAGIC + MB_FLAGS)

/* the multiboot-header */
.align 4
.int    MB_MAGIC
.int    MB_FLAGS
.int    MB_CHECKSUM

.section .text

/* Init is placed in init.c */
.extern init

/*
* _start must be global,so the Linker could find it
* It will be our entry point
*/
.global _start
_start:
    /* Init stack */
    mov $kernel_stack, %esp

    /* call c-function */
	push %ebx
    call init



    /* Endless ... :)*/
    jmp .

/* 8 kB Stack for the kernel. Remember Stacks grows downwards */
.section .bss
.space 8192
kernel_stack:
