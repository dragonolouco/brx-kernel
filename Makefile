PROJECT := brx-kernel
BUILD := build
LIMINE := third_party/limine-binary
SHELL := /bin/bash
CC := gcc
LD := ld
CFLAGS := -std=c11 -O2 -Wall -Wextra -Werror -ffreestanding -fno-stack-protector -fno-pie -fno-asynchronous-unwind-tables -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -mcmodel=kernel -m64 -Iinclude
LDFLAGS := -nostdlib -z max-page-size=0x1000 -T linker/brx.ld

.PHONY: all clean iso run check check-loader
all: $(BUILD)/brx.elf

$(BUILD):
	mkdir -p $(BUILD)/iso/boot

$(BUILD)/kernel.o: src/kernel.c include/brx_boot.h include/brx_memory.h include/brx_interrupts.h include/brx_module.h include/brx_exec.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/memory.o: src/memory.c include/brx_memory.h include/brx_boot.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/interrupts.o: src/interrupts.c include/brx_interrupts.h include/brx_ps2.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/interrupts-asm.o: src/interrupts.S | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/module.o: src/module.c include/brx_module.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/pci.o: src/pci.c include/brx_pci.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/ps2.o: src/ps2.c include/brx_ps2.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/exec.o: src/exec.c include/brx_exec.h include/brx_elf.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/elf.o: src/elf.c include/brx_elf.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/brx.elf: $(BUILD)/kernel.o $(BUILD)/memory.o $(BUILD)/interrupts.o $(BUILD)/interrupts-asm.o $(BUILD)/module.o $(BUILD)/pci.o $(BUILD)/ps2.o $(BUILD)/exec.o $(BUILD)/elf.o linker/brx.ld
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/kernel.o $(BUILD)/memory.o $(BUILD)/interrupts.o $(BUILD)/interrupts-asm.o $(BUILD)/module.o $(BUILD)/pci.o $(BUILD)/ps2.o $(BUILD)/exec.o $(BUILD)/elf.o

$(BUILD)/test-exec-loader: tools/test-exec-loader.c src/exec.c src/elf.c include/brx_exec.h include/brx_elf.h | $(BUILD)
	$(CC) -std=c11 -O2 -Wall -Wextra -Werror -Iinclude tools/test-exec-loader.c src/exec.c src/elf.c -o $@

check-loader: $(BUILD)/test-exec-loader
	$(BUILD)/test-exec-loader

$(BUILD)/iso/boot/brx.elf: $(BUILD)/brx.elf | $(BUILD)
	cp $< $@

$(BUILD)/iso/boot/limine/limine.conf: limine.conf | $(BUILD)
	mkdir -p $(BUILD)/iso/boot/limine
	cp $< $(BUILD)/iso/boot/limine/limine.conf

$(LIMINE)/limine: $(LIMINE)/limine.c
	$(CC) -g -O2 -pipe -std=c99 -D_FILE_OFFSET_BITS=64 $< -o $@

iso: $(BUILD)/iso/boot/brx.elf $(BUILD)/iso/boot/limine/limine.conf $(LIMINE)/limine
	cp $(LIMINE)/limine-bios.sys $(BUILD)/iso/boot/limine/
	cp $(LIMINE)/limine-bios-cd.bin $(BUILD)/iso/boot/limine/
	cp $(LIMINE)/limine-uefi-cd.bin $(BUILD)/iso/boot/limine/
	mkdir -p $(BUILD)/iso/EFI/BOOT
	cp $(LIMINE)/BOOTX64.EFI $(BUILD)/iso/EFI/BOOT/
	xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label $(BUILD)/iso -o $(BUILD)/brx.iso
	$(LIMINE)/limine bios-install $(BUILD)/brx.iso

run: iso
	timeout 8s qemu-system-x86_64 -M q35 -m 256M -cdrom $(BUILD)/brx.iso -serial stdio -display none -no-reboot -no-shutdown

check: check-loader iso
	rm -f $(BUILD)/qemu.log
	timeout 8s qemu-system-x86_64 -M q35 -m 256M -cdrom $(BUILD)/brx.iso -boot d -serial file:$(BUILD)/qemu.log -display none -no-reboot -no-shutdown || test $$? -eq 124
	grep -q 'BRX bootstrap complete' $(BUILD)/qemu.log

clean:
	rm -rf $(BUILD)
