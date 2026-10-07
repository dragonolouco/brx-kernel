#!/usr/bin/env bash
set -euo pipefail
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root"

check() {
    local label="$1"; shift
    if "$@" >/dev/null 2>&1; then
        printf 'implemented  %-28s\n' "$label"
    else
        printf 'missing      %-28s\n' "$label"
    fi
}

printf 'BRX Kernel audit\n================\n'
check 'x86_64 boot' test -f src/kernel.c
check 'Limine requests' grep -q 'limine_requests' src/kernel.c
check 'physical page allocator' test -f src/memory.c
check 'IDT/PIC/PIT' test -f src/interrupts.c
check 'assembly ISR stubs' test -f src/interrupts.S
check 'PCI hardware bus' test -f src/pci.c
check 'PS/2 keyboard/mouse driver' test -f src/ps2.c
check 'hardware matrix' test -f docs/HARDWARE.md
check 'module ABI registry' test -f include/brx_module.h
check 'internal application loader' test -f src/exec.c
check 'clean-room PE parser' test -f src/pe.c
check 'virtual memory regions' test -f src/vmem.c
check 'process model' test -f src/process.c
check 'compatibility architecture' test -f docs/ARCHITECTURE.md
check 'automated QEMU test' grep -q 'make check' README.md
check 'no kernel version branding' bash -c "! grep -q 'BRX Kernel 0\\.' src/kernel.c README.md"

printf '\nplanned compatibility modules\n-----------------------------\n'
printf '%s\n' '- compat.posix: process, file, pipe and terminal API'
printf '%s\n' '- compat.linux-abi: Linux syscall translation layer'
printf '%s\n' '- user.git: native Git after VFS, network, TLS and user space'
printf '%s\n' '- user.shell: command interpreter and module/package manager'
