# BRX Kernel

**BRX Kernel** é um núcleo experimental próprio para x86_64. Esta primeira versão inicializa em uma máquina virtual real via Limine, configura uma saída serial, recebe o mapa de memória do firmware/bootloader e entra em um laço de espera do próprio kernel.

## O que é próprio

- Código de entrada do núcleo e lógica de diagnóstico em `src/kernel.c`.
- Contrato mínimo de boot em `include/brx_boot.h`.
- Organização de memória e linkedição em `linker/brx.ld`.
- Build, imagem ISO e teste automatizado em `Makefile`.

O arquivo Linux anexado não é usado como base de código, não é compilado e não é copiado para este repositório. Ele pode servir apenas como referência conceitual para estudar arquitetura de sistemas operacionais.

## Dependência externa

O BRX usa o **Limine v12.9.3** somente como bootloader. O bootloader é uma camada externa que carrega o ELF do BRX; ele não é o kernel BRX. A distribuição binária fixada fica em `third_party/limine-binary`.

## Build e teste

Dependências Ubuntu: `gcc`, `binutils`, `xorriso`, `qemu-system-x86`, `nasm`, `clang`, `lld`, `llvm`, `autoconf` e `automake`.

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
BRX Kernel 0.1.0
status: native x86_64 kernel online
memory regions: 0x0000000000000014
usable memory: 0x000000000fe87000 bytes
status: BRX bootstrap complete
```

O teste `make check` executa a ISO no QEMU sem interface gráfica e verifica a mensagem `BRX bootstrap complete` na porta serial.

## Estado atual

Versão `0.1.0`: boot, console serial e mapa de memória. Ainda não há scheduler, gerenciador de páginas, interrupções, drivers, sistema de arquivos ou user space. Esses serão subsistemas posteriores, não serão fingidos nesta entrega.

## Licença

O código original do BRX é distribuído sob MIT. A dependência Limine mantém seus próprios arquivos de licença.
