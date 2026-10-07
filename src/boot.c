#include "brx_boot.h"

int brx_boot_info_valid(const brx_boot_info_t *info) {
    if (!info || info->magic != BRX_BOOT_MAGIC || info->abi != BRX_BOOT_ABI) return 0;
    if (info->memory_count == 0 || info->memory_count > 4096 || info->memory_map == 0) return 0;
    if (info->kernel_base == 0 || info->kernel_length == 0) return 0;
    if (info->kernel_base > UINT64_MAX - info->kernel_length) return 0;
    if (info->framebuffer_address && (info->framebuffer_width == 0 || info->framebuffer_height == 0 || info->framebuffer_pitch == 0)) return 0;
    return 1;
}
