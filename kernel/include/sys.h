#include <stdint.h>
#include "types.h"

#define NO_TASK 0
#define TASK_DELETED 1
#define PERMISSON_DENIED 2

uint32_t sys_getPid();
uint8_t deleteTask(uint32_t pid);
struct task* init_task(void* entry);