#include <stdint.h>
#include <string.h>
#include "mm.h"
#include "multiboot.h"

/*
 * to simplify the mm maping proccess we use a static size of 4Gb Ram
 * -> 4 GB / 4 kB = 1M Bits; 1M Bits are 1M/32 = 32k Bitmap array entries
 *
 * 1 = memory free, 0 = memory alloced
 */
#define BITMAP_SIZE 32768
static uint32_t bitmap[BITMAP_SIZE];

static void pmm_mark_used(void* page);

extern const void kernel_start;
extern const void kernel_end;

void pmm_init(struct multiboot_info* mb_info)
{
    struct multiboot_mmap* mmap = mb_info->mbs_mmap_addr;
    struct multiboot_mmap* mmap_end = (void*)
        ((uintptr_t) mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length);

    /* at begin all is reserved */
    memset(bitmap, 0, sizeof(bitmap));

    /*
     * the BIOS-memory-map deklares free memory
     */
    while (mmap < mmap_end)
	{
        if (mmap->type == 1)
		{
            /* it's free, so set the bit */
            uintptr_t addr = mmap->base;
            uintptr_t end_addr = addr + mmap->length;

            while (addr < end_addr)
			{
                pmm_free((void*) addr);
                addr += 0x1000;
            }
        }
        mmap++;
    }

    /* IMPORTANT: mark kernel space as alloc'd */
    uintptr_t addr = (uintptr_t) &kernel_start;
    while (addr < (uintptr_t) &kernel_end)
	{
        pmm_mark_used((void*) addr);
        addr += 0x1000;
    }

    /*
	* lets mark the multiboot struct and the module list too.
	* Let's say both stay under 4k TODO make real solution
	*/
	     struct multiboot_module* modules = mb_info->mbs_mods_addr;

	     pmm_mark_used(mb_info);
	     pmm_mark_used(modules);

	     /* naturally the modules themself are used too */
	     int i;
	     for (i = 0; i < mb_info->mbs_mods_count; i++)
		 {
			addr = modules[i].mod_start;
			while (addr < modules[i].mod_end)
			{
				pmm_mark_used((void*) addr);
				addr += 0x1000;
			}
		}
}

void* pmm_alloc(void)
{
    int i, j;

    /*
     * First we search in all entry, wenn a entry != 0 there is surely one piece of free memory
     */
    for (i = 0; i < BITMAP_SIZE; i++) {
        if (bitmap[i] != 0)
		{

            /* so lets find the free bit (the set bit)) */
            for (j = 0; j < 32; j++)
			{
                if (bitmap[i] & (1 << j))
				{
                    bitmap[i] &= ~(1 << j);
                    return (void*)( (i * 32 + j) * 4096);
                }
            }
        }
    }

    /* no memory free */
    return NULL;
}

static void pmm_mark_used(void* page)
{
    uintptr_t index = (uintptr_t) page / 4096;
    bitmap[index / 32] &= ~(1 << (index % 32));
}

void pmm_free(void* page)
{
    uintptr_t index = (uintptr_t) page / 4096;
    bitmap[index / 32] |= (1 << (index % 32));
}
