# BRX Kernel — expansão de sistema

## Já executa

- Entrada x86_64 via bootloader externo.
- Console serial para diagnóstico.
- Leitura do mapa de memória.
- Alocador físico de páginas.
- IDT básica, PIC remapeado e PIT periódico.
- Enumeração de dispositivos PCI por configuração I/O.
- Drivers PS/2 iniciais com IRQ de teclado e mouse.
- Registro de módulos com estados `ready` e `planned`.

## Bloco A — fundação do kernel

- [x] Entrada e linker x86_64
- [x] IDT/PIC/PIT
- [x] Alocador físico
- [ ] Gerenciador de páginas virtuais e higher-half mapping próprio
- [ ] Heap do kernel com `kmalloc/kfree`
- [ ] Exceções específicas, panic, backtrace e dump de registradores
- [ ] SMP/APIC, descoberta de CPUs e sincronização
- [ ] Scheduler preemptivo, threads do kernel e sleep/wakeup
- [ ] IPC, filas, locks, semáforos e futex BRX

## Bloco B — hardware e drivers

- [x] Enumeração PCI
- [x] PS/2 teclado e mouse iniciais
- [ ] PCI BARs, MSI/MSI-X e DMA seguro
- [ ] Console framebuffer e terminal virtual
- [ ] ACPI: tabelas RSDP/RSDT/XSDT, MADT e HPET
- [ ] APIC local/IOAPIC
- [ ] RTC e relógio monotônico
- [ ] Virtio-blk e armazenamento em bloco
- [ ] AHCI/SATA como driver opcional
- [ ] Virtio-net, e1000 e rede modular
- [ ] USB host controller e HID
- [ ] Áudio, GPU e energia como módulos opcionais

## Bloco C — serviços BRX

- [ ] VFS e inodes
- [ ] initramfs e formato de pacote BRX
- [ ] Sistema de arquivos nativo BRX
- [ ] FAT/ISO9660 para intercâmbio
- [ ] permissões, usuários, grupos e capabilities
- [ ] `/dev`, `/proc` e `/sys` BRX
- [ ] init/service manager e shell
- [ ] logs estruturados e observabilidade

## Bloco D — compatibilidade Linux

A compatibilidade é uma camada, não cópia do Linux:

1. ABI POSIX de processos, arquivos, terminal e sinais.
2. Loader ELF e runtime C.
3. Tradução de syscalls Linux para serviços BRX.
4. `/proc`, `/dev`, sockets, threads, epoll e futex compatíveis.
5. Ferramentas Linux recompiladas para BRX.
6. Execução de binários Linux sem recompilar, quando os contratos estiverem completos.

## Git

Git pertence ao user space. Para suportá-lo de forma real, o BRX precisa primeiro de VFS, processos, memória virtual, pipes, sockets, TLS/SSH, relógio, aleatoriedade e uma libc. O módulo `user.git` fica planejado até esses contratos existirem; isso é mais honesto do que colocar um falso “Git” no kernel.
