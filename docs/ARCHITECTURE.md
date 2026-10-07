# Arquitetura BRX

## Objetivo

O BRX é um sistema operacional próprio com uma arquitetura modular. A semelhança com Linux estará nas interfaces que o usuário espera encontrar — processos, arquivos, rede, terminal, permissões e ferramentas — e não no código interno do kernel. O caminho principal será adaptar código-fonte e recompilar para BRX, não emular binários.

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

## Compatibilidade por código-fonte

Quando o código-fonte estiver disponível, a preferência é adaptar headers, libc, syscalls, filesystem, rede, gráficos e input para o SDK BRX. O resultado será um ELF nativo compilado para o alvo BRX. Linguagens e engines não serão executadas por uma tradução contínua: seus compiladores, interpretadores ou runtimes serão portados para consumir os serviços BRX.

Esse modelo permite portar C, C++, Rust, Go, Python, Lua e engines gradualmente, começando por uma toolchain externa que gere programas BRX e depois chegando a um BRX self-hosting que compile seus próprios programas.

## Compatibilidade Linux opcional

Quando não houver código-fonte ou quando um port for inviável, uma camada opcional poderá reduzir o esforço:

1. **Ferramentas recompiladas**: caminho padrão; portar utilitários para a API BRX.
2. **Compatibilidade de syscalls**: caminho auxiliar para reduzir alterações no código.
3. **Binários ELF Linux sem recompilar**: último recurso, exigindo loader, sinais, threads, `/proc`, `/dev`, sockets e demais contratos.

Não devemos prometer que um binário Linux roda apenas porque o kernel inicia. A compatibilidade é um produto de vários contratos, não um arquivo copiado do Linux.

## Git

Git será uma ferramenta de user space, não uma parte do kernel. Para executar Git nativamente no BRX, serão necessários:

- processos e espaço de usuário;
- syscalls POSIX/BRX para arquivos, diretórios, pipes, sinais e memória;
- VFS e um sistema de arquivos persistente;
- sockets TCP/TLS ou um cliente SSH;
- relógio, aleatoriedade, permissões e armazenamento;
- port da biblioteca C e do runtime usado pelo Git.

O módulo `user.git` é registrado como planejado para tornar essa dependência visível. O objetivo é recompilar Git para BRX contra `brx-libc`, `brx-vfs`, `brx-net` e `brx-thread`. A execução de um binário Linux sem recompilar fica fora do caminho principal.

## ABI de módulos

`include/brx_module.h` define a primeira ABI estática. Cada módulo tem nome, versão, ABI, capacidades e estado. A ABI será expandida para dependências, init/fini, eventos, isolamento e carregamento de módulos externos.

O registro atual não finge que compatibilidade Linux ou Git já estão prontos: módulos incompletos permanecem explicitamente em estado `PLANNED`.
