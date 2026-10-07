#include "brx_pe.h"

#define PE_MACHINE_X86_64 0x8664
#define PE_OPTIONAL_MAGIC_PE32_PLUS 0x20b
#define PE_SECTION_EXECUTABLE 0x20000000U
#define PE_SIGNATURE_SIZE 4
#define PE_COFF_SIZE 20
#define PE_OPTIONAL_MIN_SIZE 112
#define PE_SECTION_SIZE 40

static int range_ok(uint64_t length, uint64_t offset, uint64_t size) {
    return offset <= length && size <= length - offset;
}

static uint16_t u16(const uint8_t *p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t u32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint64_t u64(const uint8_t *p) {
    return (uint64_t)u32(p) | ((uint64_t)u32(p + 4) << 32);
}

int brx_pe_validate_x86_64(const void *image, uint64_t length, brx_pe_image_t *result) {
    if (!image || !result || length < 0x40) return 0;
    const uint8_t *bytes = (const uint8_t *)image;
    result->image_base = 0;
    result->entry_address = 0;
    result->image_size = 0;
    result->header_size = 0;
    result->section_count = 0;
    result->executable_sections = 0;

    if (bytes[0] != 'M' || bytes[1] != 'Z') return 0;
    uint32_t pe_offset = u32(bytes + 0x3c);
    if (!range_ok(length, pe_offset, PE_SIGNATURE_SIZE + PE_COFF_SIZE)) return 0;
    if (bytes[pe_offset] != 'P' || bytes[pe_offset + 1] != 'E' || bytes[pe_offset + 2] != 0 || bytes[pe_offset + 3] != 0) return 0;

    const uint8_t *coff = bytes + pe_offset + PE_SIGNATURE_SIZE;
    uint16_t machine = u16(coff);
    uint16_t section_count = u16(coff + 2);
    uint16_t optional_size = u16(coff + 16);
    if (machine != PE_MACHINE_X86_64 || section_count == 0 || optional_size < PE_OPTIONAL_MIN_SIZE) return 0;

    const uint8_t *optional = coff + PE_COFF_SIZE;
    if (!range_ok(length, (uint64_t)(optional - bytes), optional_size)) return 0;
    if (u16(optional) != PE_OPTIONAL_MAGIC_PE32_PLUS) return 0;
    uint32_t entry_rva = u32(optional + 16);
    uint64_t image_base = u64(optional + 24);
    uint32_t size_image = u32(optional + 56);
    uint32_t size_headers = u32(optional + 60);
    uint32_t section_alignment = u32(optional + 32);
    if (size_image == 0 || size_headers == 0 || section_alignment == 0 || size_headers > size_image) return 0;
    if (image_base > UINT64_MAX - entry_rva) return 0;

    uint64_t sections_offset = (uint64_t)(optional - bytes) + optional_size;
    if (!range_ok(length, sections_offset, (uint64_t)section_count * PE_SECTION_SIZE)) return 0;
    result->image_base = image_base;
    result->entry_address = image_base + entry_rva;
    result->image_size = size_image;
    result->header_size = size_headers;
    result->section_count = section_count;

    int entry_in_executable = 0;
    for (uint16_t i = 0; i < section_count; ++i) {
        const uint8_t *section = bytes + sections_offset + (uint64_t)i * PE_SECTION_SIZE;
        uint32_t virtual_size = u32(section + 8);
        uint32_t virtual_address = u32(section + 12);
        uint32_t raw_size = u32(section + 16);
        uint32_t raw_offset = u32(section + 20);
        uint32_t characteristics = u32(section + 36);
        if (!range_ok(length, raw_offset, raw_size)) return 0;
        if ((uint64_t)virtual_address + virtual_size > size_image) return 0;
        if (characteristics & PE_SECTION_EXECUTABLE) {
            result->executable_sections++;
            if (entry_rva >= virtual_address && entry_rva < (uint64_t)virtual_address + virtual_size) entry_in_executable = 1;
        }
    }
    return result->executable_sections != 0 && entry_in_executable;
}
