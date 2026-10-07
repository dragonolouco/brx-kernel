#include "brx_elf.h"

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ET_EXEC 2
#define ET_DYN 3
#define EM_X86_64 62
#define PT_LOAD 1
#define PF_W 2
#define ELF_HEADER_SIZE 64
#define ELF_PROGRAM_HEADER_SIZE 56

typedef struct {
    uint8_t ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} brx_elf64_header_t;

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} brx_elf64_program_header_t;

static int range_ok(uint64_t length, uint64_t offset, uint64_t size) {
    return offset <= length && size <= length - offset;
}

int brx_elf_validate_x86_64(const void *image, uint64_t length, brx_elf_image_t *result) {
    if (!image || !result || length < ELF_HEADER_SIZE) return 0;
    const brx_elf64_header_t *header = (const brx_elf64_header_t *)image;
    result->entry = 0;
    result->lowest_load_address = UINT64_MAX;
    result->highest_load_address = 0;
    result->loadable_segments = 0;
    result->memory_size = 0;
    result->is_position_independent = header->type == ET_DYN;

    if (header->ident[0] != 0x7f || header->ident[1] != 'E' || header->ident[2] != 'L' || header->ident[3] != 'F') return 0;
    if (header->ident[4] != ELFCLASS64 || header->ident[5] != ELFDATA2LSB) return 0;
    result->entry = header->entry;
    if ((header->type != ET_EXEC && header->type != ET_DYN) || header->machine != EM_X86_64) return 0;
    if (header->ehsize < ELF_HEADER_SIZE || header->phentsize < ELF_PROGRAM_HEADER_SIZE || header->phnum == 0) return 0;
    if (!range_ok(length, header->phoff, (uint64_t)header->phentsize * header->phnum)) return 0;

    for (uint16_t i = 0; i < header->phnum; ++i) {
        const uint8_t *raw = (const uint8_t *)image + header->phoff + (uint64_t)i * header->phentsize;
        const brx_elf64_program_header_t *program = (const brx_elf64_program_header_t *)raw;
        if (program->type != PT_LOAD) continue;
        if (program->memsz < program->filesz || !range_ok(length, program->offset, program->filesz)) return 0;
        if (program->vaddr > UINT64_MAX - program->memsz) return 0;
        if (program->vaddr < result->lowest_load_address) result->lowest_load_address = program->vaddr;
        if (program->vaddr + program->memsz > result->highest_load_address) result->highest_load_address = program->vaddr + program->memsz;
        result->loadable_segments++;
    }

    if (result->loadable_segments == 0 || result->entry < result->lowest_load_address || result->entry >= result->highest_load_address) return 0;
    result->memory_size = result->highest_load_address - result->lowest_load_address;
    return 1;
}
