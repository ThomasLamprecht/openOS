#include "console.h"
#include "intr.h"
#include "mm.h"

struct task {
    struct cpu_state*   cpu_state;
    struct task*        next;
};
 
static struct task* first_task = NULL;
static struct task* current_task = NULL;

void idle()
{
	while(1){/*Waiting so hard*/}
}
static void task_a(void)
{
    uint64_t j;
    
    cmos_write(0x0b,0x04);
    while(1)
    {
        kprintf("A");
        for(j=0;j<20000;j++)
    		continue;
    }
}

static void task_b(void)
{
    uint64_t j;
    while(1)
    {
        kprintf("B");
        for(j=0;j<200000;j++)
    		continue;
    }
}

static void task_c(void)
{
    uint64_t j;
    while(1)
    {
        kprintf("C");
        for(j=0;j<200000;j++)
    		continue;
    }
}

/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct task* init_task(void* entry)
{
	uint8_t* stack = pmm_alloc();
	uint8_t* user_stack = pmm_alloc();
    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    struct cpu_state new_state = {
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
    task->next = first_task;
    first_task = task;
    return task;
}

void init_multitasking(void)
{
    init_task(task_a);
    init_task(task_b);
	init_task(task_c);
	//task_states[4] = init_task(stack_c, user_stack_c, task_c);
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
