#include <stdint.h>
#include "intr.h"
#include "console.h"

// Exceptions
extern void intr_stub_0(void);
extern void intr_stub_1(void);
extern void intr_stub_2(void);
extern void intr_stub_3(void);
extern void intr_stub_4(void);
extern void intr_stub_5(void);
extern void intr_stub_6(void);
extern void intr_stub_7(void);
extern void intr_stub_8(void);
extern void intr_stub_9(void);
extern void intr_stub_10(void);
extern void intr_stub_11(void);
extern void intr_stub_12(void);
extern void intr_stub_13(void);
extern void intr_stub_14(void);
extern void intr_stub_15(void);
extern void intr_stub_16(void);
extern void intr_stub_17(void);
extern void intr_stub_18(void);

// IRQ's
extern void intr_stub_32(void);
extern void intr_stub_33(void);
extern void intr_stub_34(void);
extern void intr_stub_35(void);
extern void intr_stub_36(void);
extern void intr_stub_37(void);
extern void intr_stub_38(void);
extern void intr_stub_39(void);
extern void intr_stub_40(void);
extern void intr_stub_41(void);
extern void intr_stub_42(void);
extern void intr_stub_43(void);
extern void intr_stub_44(void);
extern void intr_stub_45(void);
extern void intr_stub_46(void);
extern void intr_stub_47(void);

// Syscalls (software int's)
extern void intr_stub_48(void);

#define GDT_FLAG_DATASEG 0x02
#define GDT_FLAG_CODESEG 0x0a
#define GDT_FLAG_TSS     0x09

#define GDT_FLAG_SEGMENT 0x10
#define GDT_FLAG_RING0   0x00
#define GDT_FLAG_RING3   0x60
#define GDT_FLAG_PRESENT 0x80

#define GDT_FLAG_4K      0x800
#define GDT_FLAG_32_BIT  0x400

#define GDT_ENTRIES 6
static uint64_t gdt[GDT_ENTRIES];
static uint32_t tss[32] = { 0, 0, 0x10 };

#define IDT_ENTRIES 256
static long long unsigned int idt[IDT_ENTRIES];

static void gdt_set_entry(int i, unsigned int base, unsigned int limit, int flags)
{
    gdt[i] = limit & 0xffffLL;
    gdt[i] |= (base & 0xffffffLL) << 16;
    gdt[i] |= (flags & 0xffLL) << 40;
    gdt[i] |= ((limit >> 16) & 0xfLL) << 48;
    gdt[i] |= ((flags >> 8 )& 0xffLL) << 52;
    gdt[i] |= ((base >> 24) & 0xffLL) << 56;
}

void init_gdt(void)
{
    struct
    {
        uint16_t limit;
        void* pointer;
    } __attribute__((packed)) gdtp =
    {
        .limit = GDT_ENTRIES * 8 - 1,
        .pointer = gdt,
    };

    // build GDT-entries
    gdt_set_entry(0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
    gdt_set_entry(2, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT);
    gdt_set_entry(3, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_CODESEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
    gdt_set_entry(4, 0, 0xfffff, GDT_FLAG_SEGMENT | GDT_FLAG_32_BIT | GDT_FLAG_DATASEG | GDT_FLAG_4K | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
    gdt_set_entry(5, (uint32_t)tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);

    // reload GDT
    asm volatile("lgdt %0" : : "m" (gdtp));

    // reload segment-registers, so GDT-entries will be used
    asm volatile(
        "mov $0x10, %ax;"
        "mov %ax, %ds;"
        "mov %ax, %es;"
        "mov %ax, %ss;"
        "ljmp $0x8, $.1;"
        ".1:"
    );
    // reload task register
    asm volatile("ltr %%ax" : : "a" (5 << 3));
}

#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT 0x80
#define IDT_FLAG_RING0 0x00
#define IDT_FLAG_RING3 0x60

static void idt_set_entry(int i, void (*fn)(), unsigned int selector, int flags)
{
    unsigned long int handler = (unsigned long int) fn;
    idt[i] = handler & 0xffffLL;
    idt[i] |= (selector & 0xffffLL) << 16;
    idt[i] |= (flags & 0xffLL) << 40;
    idt[i] |= ((handler>> 16) & 0xffffLL) << 48;
}

/* writes a byte on an I/O-Port */ // TODO move
static inline void outb(uint16_t port, uint8_t data)
{
    asm volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}

/* gets a byte from an I/O-Port */
static inline uint8_t inb(uint16_t port)
{
	uint8_t result=0;
    asm volatile ("inb %1, %0" : : "a" (result), "Nd" (port));
    return result;
}

// TODO tidy up -> move
uint8_t cmos_read(uint8_t offset)
{
  uint8_t tmp = inb(0x70);
  outb(0x70, (tmp & 0x80) | (offset & 0x7F));
  return inb(0x71);
}
void cmos_write(uint8_t offset,uint8_t val) {
  uint8_t tmp = inb(0x70);
  outb(0x70, (tmp & 0x80) | (offset & 0x7F));
  outb(0x71,val);
}

static void init_pic(void)
{
    // initialize master-PIC
    outb(0x20, 0x11); // Initialize-command for the PIC
    outb(0x21, 0x20); // Interrupt number for IRQ 0
    outb(0x21, 0x04); // On IRQ 2 hangs the slave
    outb(0x21, 0x01); // ICW 4

	// initialize slave-PIC
	outb(0xa0, 0x11); // Initialize-command for the PIC
	outb(0xa1, 0x28); // Interrupt number for IRQ 8
	outb(0xa1, 0x02); // On IRQ 2 hangs the Slave
    outb(0xa1, 0x01); // ICW 4

    // activate all IRQs (demasking them)
    outb(0x20, 0x0);
    outb(0xa0, 0x0);
}


void init_intr(void)
{
    struct
    {
        unsigned short int limit;
        void* pointer;
    } __attribute__((packed)) idtp = {
        .limit = IDT_ENTRIES * 8 - 1,
        .pointer = idt,
    };

	// redirect interrupt numbers of the IRQs
    init_pic();

    // Excpetion-handler
    idt_set_entry(0, intr_stub_0, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(1, intr_stub_1, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(2, intr_stub_2, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(3, intr_stub_3, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(4, intr_stub_4, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(5, intr_stub_5, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(6, intr_stub_6, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(7, intr_stub_7, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(8, intr_stub_8, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(9, intr_stub_9, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(10, intr_stub_10, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(11, intr_stub_11, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(12, intr_stub_12, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(13, intr_stub_13, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(14, intr_stub_14, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(15, intr_stub_15, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(16, intr_stub_16, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(17, intr_stub_17, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(18, intr_stub_18, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

    // IRQ-handler
    idt_set_entry(32, intr_stub_32, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(33, intr_stub_33, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(34, intr_stub_34, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(35, intr_stub_35, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(36, intr_stub_36, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(37, intr_stub_37, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(38, intr_stub_38, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(39, intr_stub_39, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(40, intr_stub_40, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(41, intr_stub_41, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(42, intr_stub_42, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(43, intr_stub_43, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(44, intr_stub_44, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(45, intr_stub_45, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(46, intr_stub_46, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
	idt_set_entry(47, intr_stub_47, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

    // Syscall
    idt_set_entry(48, intr_stub_48, 0x8, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT);

    asm volatile("lidt %0" : : "m" (idtp)); // reload idt

    asm volatile("sti"); // enable interrupts
}

struct cpu_state* handle_interrupt(struct cpu_state* cpu)
 {
    struct cpu_state* new_cpu = cpu;
    if (cpu->intr <= 0x1f)
    {
    	const char *emesg[] =
    	{
			"'Divide by zero' occurred",
			"'Single step' occurred, debugging pls",
			"'Non maskable' occurred",
			"'Breakpoint Exception' occurred",
			"'Overflow Exception' occured",
			"BOUND range exceeded",
			"Invalid opcode",
			"Coprocessor not available", // we should emulate it, but I don't think that thus exception occurs nowadays
			"'Double fault' occurred",
			"Coprocessor segment overrun",
			"Invalid task state segment",
			"Segment not present",
			"Stack exception",
			"General protection exception",
			"Page fault", // Shouldn't occur 'til we've implemented paging ;)
			"Reserved o.O",
			"Coprocessor error"
    	};
		kprintf("\nE %d, Panic: \"%s\" eno: %x\n", cpu->intr, emesg[cpu->intr],cpu->error);
		kprintf("eax: 0x%x;\t"
		"ebx: 0x%x;\n"
		"ecx: 0x%x;\t"
		"edx: 0x%x;\n"
		"esi: 0x%x;\t"
		"edi: 0x%x;\n"
		"ebp: 0x%x;\t"
		"eip: 0x%x;\n"
		"cs : 0x%x;\t"
		"eflags: 0x%x;\n"
		"esp: 0x%x;\t"
		"ss : 0x%x\n",cpu->eax,cpu->ebx,cpu->ecx,cpu->edx,cpu->esi,cpu->edi,cpu->ebp,cpu->eip,cpu->cs,cpu->eflags,cpu->esp,cpu->ss);

        while(1)
        {
            asm volatile("cli; hlt"); // stopping cpu, we should kill the guilty task and go one
        }
    }
    else if (cpu->intr >= 0x20 && cpu->intr <= 0x2f)
    {
        if (cpu->intr == 0x20) // timer interrupt, let's schedule
        {
			new_cpu = schedule(cpu);
			tss[1] = (uint32_t) (new_cpu + 1);
		}
        if (cpu->intr >= 0x28)
        {
            // End Of Interrupt to slave-PIC
            outb(0xa0, 0x20);
        }
        // EOI to master-PIC
        outb(0x20, 0x20);
    }
	else if (cpu->intr == 0x30)
	{
		new_cpu = syscall(cpu);
	}
    else
    {
        kprintf("Unknown interrupt \"%d\"\n",cpu->intr);
        while(1)
        {
            asm volatile("cli; hlt"); // Prozessor anhalten
        }
    }
    return new_cpu;
}
