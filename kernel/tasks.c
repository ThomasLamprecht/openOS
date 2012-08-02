#include <stddef.h>
#include <string.h>

#include "console.h"
#include "intr.h"
#include "mm.h"
#include "multiboot.h"
#include "syscall.h"
#include "sys.h"
#include "asm/io.h"

static struct task* first_task = NULL;
static struct task* current_task = NULL;
static uint32_t max_pid = 0;

static void idle(void)
{
//	asm volatile ("hlt"); // saves power
	asm ("int $0x30"::"a"(SYSCALL_YIELD)); // Should now be faster wenn idle;
	while(1){/*Waiting so hard*/}
}
static void task_a(void)
{
    uint64_t j;

	kprintf("Task A started\n");
	for(j=0;j<200;j++)
		kprintf("A");//continue;
	kprintf("Task A stopped\n");
	asm ("int $0x30"::"a"(SYSCALL_EXIT), "b"(1));
	idle();
}

static void task_b(void)
{
    uint64_t j;

	kprintf("Task B (%d) started\n",sys_getPid());
	for(j=0;j<250;j++)
		kprintf("B");//continue;
	kprintf("Task B (%d) stopped\n",sys_getPid());

	asm ("int $0x30"::"a"(SYSCALL_EXIT), "b"(0));
	idle();
}

static void task_c(void)
{
	int j;
	uint8_t sec,min,hour;
	while(1)
	{
		outb(0x70,0x04);
		hour = inb(0x71);
		outb(0x70,0x02);
		min = inb(0x71);
		outb(0x70,0x00);
		sec = inb(0x71);
		
		kprintf("%d:%d:%d\n",hour,min,sec);
		for(j=0;j<8000000;j++)
			continue;
	}
	asm ("int $0x30"::"a"(SYSCALL_EXIT), "b"(0));
	idle();
}

/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct task* init_task(void* entry)
{
	uint8_t* stack = pmm_alloc();
	uint8_t* user_stack = pmm_alloc(); // Tasks has 4 KB of stack....
    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    struct cpu_state new_state =
    {
        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0,
        .ebp = 0,
        .esp = (uint32_t) user_stack + 4096, // doing user task: yeah :)
        .eip = (uint32_t) entry,

        /* Ring-0-Segmentregister */
        .cs  = 0x18 | 0x03,
        .ss  = 0x20 | 0x03, // another yeah requierd here :)

        /* IRQs einschalten (IF = 1) */
        .eflags = 0x200,
    };

    /*
     * Den angelegten CPU-Zustand auf den Stack des Tasks kopieren, damit es am
     * Ende so aussieht als waere der Task durch einen Interrupt unterbrochen
     * worden. So kann man dem Interrupthandler den neuen Task unterschieben
     * und er stellt einfach den neuen Prozessorzustand "wieder her".
     */
    struct cpu_state* state = (void*) (stack + 4096 - sizeof(new_state));
    *state = new_state;

    struct task* task = pmm_alloc();
    task->cpu_state = state;
	task->pid = max_pid++;
    task->next = first_task;
    first_task = task;
    return task;
}

void init_multitasking(struct multiboot_info* multiboot_nfo)
{
	init_task(idle);
	if (multiboot_nfo->mbs_mods_count == 0)
	{
		kprintf("No modules loaded. Starting test tasks\n");
			init_task(task_c);
			init_task(task_b);
			init_task(task_a);
	}
	else
	{

		kprintf("loading module 1 of %d\n",multiboot_nfo->mbs_mods_count);

		struct multiboot_module* modules = multiboot_nfo->mbs_mods_addr;
		size_t length = modules[0].mod_end - modules[0].mod_start;
		void* load_addr = (void*) 0x200000;

		memcpy(load_addr, (void*) modules[0].mod_start, length);
		init_task(load_addr);
	}
	//task_states[4] = init_task(stack_c, user_stack_c, task_c);
}


uint32_t sys_getPid()
{
	return current_task->pid;
}

uint8_t deleteTask(uint32_t pid)
{
	if(pid==0)
	{
		kprintf("\nTASK_ZERO_PERMISSION_DENIED\n");
		return TASK_ZERO_PERMISSION_DENIED;
	}
	struct task *actual,*prev=NULL;
	for(actual = first_task; actual != NULL; actual = actual->next)
	{
		if(actual==NULL)
			return NO_TASK;
		if(actual->pid == pid)
		{
			if(prev==NULL||actual==first_task)
					first_task = actual->next;
			else
				prev->next = actual->next;
			//if(actual == current_task) should only be used for debugging, because it'll make the scheduler undeterminable
			//{
			//	asm volatile("int $0x20"); // lets schedule
			//}
			return TASK_DELETED;
		}
		prev = actual;
	}
	return NO_TASK;
}

/*
 * Gibt den Prozessorzustand des naechsten Tasks zurueck. Der aktuelle
 * Prozessorzustand wird als Parameter uebergeben und gespeichert, damit er
 * beim naechsten Aufruf des Tasks wiederhergestellt werden kann
 */
struct cpu_state* schedule(struct cpu_state* cpu)
{
    /*
     * Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
     * gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
     * wir spaeter nicht wieder.
     */
    if (current_task != NULL)
    {
        current_task->cpu_state = cpu;
    }
    else if(first_task==NULL)
	{
		kprintf("\n------------------\nNo Tasks there\n\nHalting cpu\n"); // Should going idle or something :) Best solution, ask user (needs keyboard drivers -.-)
		asm volatile("cli; hlt");
	}
    /*
     * Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
     */
	if (current_task == NULL)
	{
        current_task = first_task;
    }
    else
    {
        current_task = current_task->next;
        if (current_task == NULL)
        {
            current_task = first_task;
        }
    }

    /* Prozessorzustand des neuen Tasks aktivieren */
    cpu = current_task->cpu_state;
    return cpu;
}
