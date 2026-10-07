#include "brx_exec.h"
#include "brx_elf.h"
#include "brx_pe.h"

static uint32_t read32(const uint8_t *bytes) {
    return (uint32_t)bytes[0]
        | ((uint32_t)bytes[1] << 8)
        | ((uint32_t)bytes[2] << 16)
        | ((uint32_t)bytes[3] << 24);
}

static int has_bytes(const uint8_t *bytes, uint64_t length, uint64_t offset, uint64_t count) {
    (void)bytes;
    return offset <= length && count <= length - offset;
}

brx_exec_format_t brx_exec_detect(const void *image, uint64_t length) {
    if (!image || length == 0) return BRX_EXEC_UNKNOWN;
    const uint8_t *bytes = (const uint8_t *)image;

    if (has_bytes(bytes, length, 0, 4) && bytes[0] == 0x7f && bytes[1] == 'E' && bytes[2] == 'L' && bytes[3] == 'F') {
        /* BRX executables carry an optional BRX note later; generic ELF is Linux-compatible. */
        return BRX_EXEC_LINUX_ELF;
    }
    if (has_bytes(bytes, length, 0, 2) && bytes[0] == 'M' && bytes[1] == 'Z') {
        if (has_bytes(bytes, length, 0x3c, 4)) {
            uint32_t pe_offset = read32(bytes + 0x3c);
            if (has_bytes(bytes, length, pe_offset, 4)
                && bytes[pe_offset] == 'P' && bytes[pe_offset + 1] == 'E'
                && bytes[pe_offset + 2] == 0 && bytes[pe_offset + 3] == 0) {
                return BRX_EXEC_WINDOWS_PE;
            }
        }
    }
    if (has_bytes(bytes, length, 0, 2) && bytes[0] == '#' && bytes[1] == '!') return BRX_EXEC_SCRIPT;
    if (has_bytes(bytes, length, 0, 4) && bytes[0] == 'B' && bytes[1] == 'R' && bytes[2] == 'X' && bytes[3] == 'P') return BRX_EXEC_PACKAGE;
    return BRX_EXEC_UNKNOWN;
}

int brx_exec_validate(const void *image, uint64_t length, brx_exec_format_t format) {
    brx_elf_image_t elf;
    brx_pe_image_t pe;
    switch (format) {
    case BRX_EXEC_BRX_ELF:
    case BRX_EXEC_LINUX_ELF:
        return brx_elf_validate_x86_64(image, length, &elf);
    case BRX_EXEC_WINDOWS_PE:
        return brx_pe_validate_x86_64(image, length, &pe);
    case BRX_EXEC_SCRIPT:
    case BRX_EXEC_PACKAGE:
        return image && length >= 2;
    default:
        return 0;
    }
}

brx_exec_plan_t brx_exec_plan(brx_exec_format_t format) {
    switch (format) {
    case BRX_EXEC_BRX_ELF: return (brx_exec_plan_t){ format, BRX_EXEC_BACKEND_NATIVE, 1, "BRX native loader" };
    case BRX_EXEC_LINUX_ELF: return (brx_exec_plan_t){ format, BRX_EXEC_BACKEND_LINUX_COMPAT, 0, "Linux source-compatible layer pending" };
    case BRX_EXEC_WINDOWS_PE: return (brx_exec_plan_t){ format, BRX_EXEC_BACKEND_WINDOWS_SOURCE, 0, "integrated Windows source runtime pending" };
    case BRX_EXEC_SCRIPT: return (brx_exec_plan_t){ format, BRX_EXEC_BACKEND_INTERPRETER, 0, "BRX interpreter registry pending" };
    case BRX_EXEC_PACKAGE: return (brx_exec_plan_t){ format, BRX_EXEC_BACKEND_PACKAGE, 0, "BRX package loader pending" };
    default: return (brx_exec_plan_t){ BRX_EXEC_UNKNOWN, BRX_EXEC_BACKEND_NONE, 0, "unknown application format" };
    }
}

const char *brx_exec_format_name(brx_exec_format_t format) {
    switch (format) {
    case BRX_EXEC_BRX_ELF: return "BRX ELF";
    case BRX_EXEC_LINUX_ELF: return "Linux ELF";
    case BRX_EXEC_WINDOWS_PE: return "Windows PE";
    case BRX_EXEC_SCRIPT: return "script";
    case BRX_EXEC_PACKAGE: return "BRX package";
    default: return "unknown";
    }
}

const char *brx_exec_backend_name(brx_exec_backend_t backend) {
    switch (backend) {
    case BRX_EXEC_BACKEND_NATIVE: return "native";
    case BRX_EXEC_BACKEND_LINUX_COMPAT: return "linux-compat";
    case BRX_EXEC_BACKEND_WINDOWS_SOURCE: return "windows-source";
    case BRX_EXEC_BACKEND_INTERPRETER: return "interpreter";
    case BRX_EXEC_BACKEND_PACKAGE: return "package";
    default: return "none";
    }
}
