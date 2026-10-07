#include <stdio.h>
#include "brx_boot.h"

int main(void) {
    brx_boot_info_t info = {
        BRX_BOOT_MAGIC, BRX_BOOT_ABI, 0x9000, 14,
        0x100000, 0x200000, 0, 0, 0, 0, 0
    };
    if (!brx_boot_info_valid(&info)) return 1;
    info.magic = 0;
    if (brx_boot_info_valid(&info)) return 2;
    puts("BRX boot contract tests: PASS");
    return 0;
}
