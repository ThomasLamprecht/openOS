#ifndef MM_H
#define MM_H

#include "multiboot.h"

#define NULL ((void*) 0)

void pmm_init(struct multiboot_info* mb_info);
void* pmm_alloc(void);
void pmm_free(void* page);

#endif
