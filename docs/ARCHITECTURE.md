# Arquitetura BRX

## Objetivo

O BRX é um sistema operacional próprio com uma arquitetura modular. A semelhança com Linux está nas interfaces que o usuário espera encontrar — processos, arquivos, rede, terminal, permissões e ferramentas — e não no código interno do kernel.

A ideia de “LEGO” significa que cada capacidade é um módulo com contrato explícito, dependências e ciclo de vida. O núcleo mínimo não precisa conter todos os drivers, sistemas de arquivos ou compatibilidades.

## Camadas

```text
Aplicações e ferramentas
        |
Compatibilidade POSIX / Linux ABI / formatos de executáveis
        |
Serviços do sistema: shell, init, VFS, rede, package manager
        |
ABI de módulos BRX: drivers, FS, rede, segurança, observabilidade
        |
Microfundação BRX: memória, interrupções, scheduler, IPC
        |
Hardware x86_64 / firmware / bootloader
```

## Compatibilidade Linux

Há três níveis possíveis, que serão implementados em ordem:

1. **Ferramentas recompiladas**: portar utilitários para a API POSIX/BRX.
2. **Compatibilidade de syscalls**: oferecer uma ABI Linux compatível e traduzir chamadas para os serviços BRX.
3. **Binários ELF Linux sem recompilar**: loader ELF, linker de runtime, sinais, threads, `/proc`, `/dev`, sockets e demais contratos necessários.

Não devemos prometer que um binário Linux roda apenas porque o kernel inicia. A compatibilidade é um produto de vários contratos, não um arquivo copiado do Linux.

## Git

Git será uma ferramenta de user space, não uma parte do kernel. Para executar Git nativamente no BRX, serão necessários:

- processos e espaço de usuário;
- syscalls POSIX/BRX para arquivos, diretórios, pipes, sinais e memória;
- VFS e um sistema de arquivos persistente;
- sockets TCP/TLS ou um cliente SSH;
- relógio, aleatoriedade, permissões e armazenamento;
- port da biblioteca C e do runtime usado pelo Git.

O módulo `user.git` é registrado como planejado para tornar essa dependência visível. Depois da camada POSIX, Git poderá ser recompilado para BRX; depois da Linux ABI, poderemos avaliar binários Linux sem recompilação.

## ABI de módulos

`include/brx_module.h` define a primeira ABI estática. Cada módulo tem nome, versão, ABI, capacidades e estado. A ABI será expandida para dependências, init/fini, eventos, isolamento e carregamento de módulos externos.

O registro atual não finge que compatibilidade Linux ou Git já estão prontos: módulos incompletos permanecem explicitamente em estado `PLANNED`.
