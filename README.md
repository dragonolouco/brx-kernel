# BRX Kernel

**BRX Kernel** é um núcleo próprio para x86_64, modular e orientado a compatibilidade. Ele não é Linux e não copia o kernel Linux. A proposta é oferecer uma base BRX independente na qual capacidades são encaixadas como módulos: drivers, memória, arquivos, rede, POSIX, compatibilidade de syscalls Linux e ferramentas de user space como Git.

## O que é próprio

- Código de entrada do núcleo e lógica de diagnóstico em `src/kernel.c`.
- Contrato mínimo de boot em `include/brx_boot.h`.
- Organização de memória e linkedição em `linker/brx.ld`.
- Alocador físico inicial em `src/memory.c`.
- IDT, PIC, PIT e IRQ de timer em `src/interrupts.c` e `src/interrupts.S`.
- ABI e registro de módulos em `include/brx_module.h` e `src/module.c`.
- Build, imagem ISO e teste automatizado em `Makefile`.

O arquivo Linux anexado não é usado como base de código, não é compilado e não é copiado para este repositório. Ele pode servir apenas como referência conceitual para estudar arquitetura de sistemas operacionais.

## Dependência externa

O BRX usa o **Limine v12.9.3** somente como bootloader. O bootloader é uma camada externa que carrega o ELF do BRX; ele não é o kernel BRX. A distribuição binária fixada fica em `third_party/limine-binary`.

## Build e teste

Dependências Ubuntu: `gcc`, `binutils`, `xorriso`, `qemu-system-x86` e `make`.

```bash
make -C third_party/limine bootstrap
make -C third_party/limine configure --enable-bios  # apenas como referência; veja abaixo
make
make check
```

O build usa os artefatos de `third_party/limine-binary` e a imagem gerada contém caminhos BIOS e UEFI. A ISO fica somente em `build/` e não é versionada.

## Verificação realizada

`make check` compilou o ELF, montou a ISO híbrida e inicializou o BRX em QEMU sem interface gráfica. A UART retornou:

```text
BRX Kernel
status: native x86_64 kernel online
memory regions: 0x0000000000000014
usable memory: 0x000000000fe87000 bytes
status: BRX bootstrap complete
```

O teste `make check` executa a ISO no QEMU sem interface gráfica e verifica a mensagem `BRX bootstrap complete` na porta serial.

## Estado atual

O núcleo atual já tem boot, console serial, mapa de memória, alocador físico, IDT/PIC/PIT, IRQ de timer, enumeração PCI, driver PS/2 inicial e registro modular. O scheduler, paginação virtual, processos, VFS, rede, user space e a camada Linux ABI ainda estão planejados; o registro informa esses módulos como `planned`, sem fingir que já estão prontos.

Leia [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) para a arquitetura de compatibilidade, [docs/PORTING.md](docs/PORTING.md) para a estratégia source-first de recompilar ferramentas e linguagens para BRX, [docs/COMPATIBILITY_RUNTIME.md](docs/COMPATIBILITY_RUNTIME.md) para o dispatcher automático de executáveis e [docs/HARDWARE.md](docs/HARDWARE.md) para os drivers.

## Licença

O código original do BRX é distribuído sob MIT. A dependência Limine mantém seus próprios arquivos de licença.
