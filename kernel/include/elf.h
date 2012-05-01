#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include "string.h"
#include "console.h"
#include "sys.h"

#define ELF_MAGIC 0x464C457F
#define NO_ELF 0
#define ELF_TASK_LOADED 1

struct elf_header {
    uint32_t    magic;
    uint32_t    version;
    uint64_t    reserved;
    uint64_t    version2;
    uint32_t    entry;
    uint32_t    ph_offset;
    uint32_t    sh_offset;
    uint32_t    flags;
    uint16_t    header_size;
    uint16_t    ph_entry_size;
    uint16_t    ph_entry_count;
    uint16_t    sh_entry_size;
    uint16_t    sh_entry_count;
    uint16_t    sh_str_table_index;
} __attribute__((packed));

struct elf_program_header {
    uint32_t    type;
    uint32_t    offset;
    uint32_t    virt_addr;
    uint32_t    phys_addr;
    uint32_t    file_size;
    uint32_t    mem_size;
    uint32_t    flags;
    uint32_t    alignment;
} __attribute__((packed));

uint8_t init_elf(void* image);

#endif