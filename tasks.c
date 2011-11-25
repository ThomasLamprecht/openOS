#include "console.h"
#include "intr.h"


void idle()
{
	asm volatile("jmp .");
}
static void task_a(void)
{
	int i;
    for(i=0;i<200;i++)
    {
        kprintf("A");
    }
    idle();
}

static void task_b(void)
{
	int i;
    for(i=0;i<200;i++)
    {
        kprintf("B");
    }
    idle();
}

static void task_c(void)
{
	int i;
	asm volatile("cli");
    for(i=0;i<200;i++)
    {
        kprintf("C");
    }
    idle();
}

static void task_d(void)
{
	int i;
    for(i=0;i<200;i++)
    {
        kprintf("D");
    }
    idle();
}

static uint8_t stack_a[4096];
static uint8_t stack_b[4096];
static uint8_t stack_c[4096];
static uint8_t stack_d[4096];

static uint8_t user_stack_a[4096];
static uint8_t user_stack_b[4096];
static uint8_t user_stack_c[4096];
static uint8_t user_stack_d[4096];
/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct cpu_state* init_task(uint8_t* stack, uint8_t* user_stack, void* entry)
{
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
        .esp = (uint32_t) user_stack, // doing user task yeah :)
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

    return state;
}

static int current_task = -1;
static int num_tasks = 4;
static struct cpu_state* task_states[4];

void init_multitasking(void)
{
    task_states[0] = init_task(stack_a, user_stack_a, task_a);
    task_states[1] = init_task(stack_b, user_stack_b, task_b);
	task_states[2] = init_task(stack_c, user_stack_c, task_c);
    task_states[3] = init_task(stack_d, user_stack_d, task_d);
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
    if (current_task >= 0)
    {
        task_states[current_task] = cpu;
    }

    /*
     * Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
     */
    current_task++;
    current_task %= num_tasks;

    /* Prozessorzustand des neuen Tasks aktivieren */
    cpu = task_states[current_task];

    return cpu;
}
