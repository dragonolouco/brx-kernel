# Portabilidade source-first do BRX

## Princípio

O caminho principal do BRX será criar implementações e portas próprias a partir de contratos públicos. Quando uma ferramenta tiver código-fonte disponível e sua licença permitir um port, ela poderá ser portada respeitando a licença; para camadas de compatibilidade e emulação, o BRX escreverá código novo em modelo clean-room, sem copiar a implementação de projetos existentes.

Isso é diferente de emulação e diferente de tradução de binário em tempo de execução:

```text
Código C/C++/Rust/Go ou engine
        |
Toolchain configurada para BRX
        |
Headers + libc + SDK BRX
        |
ELF nativo x86_64-brx
        |
Kernel e serviços BRX
```

A linguagem não “roda traduzida” dentro de um emulador. O compilador, interpretador ou runtime é portado e passa a produzir ou executar código usando os serviços do BRX.

## O que precisa existir no BRX

Para tornar esse modelo prático, o sistema precisa fornecer um alvo de compilação estável:

- convenção de chamada e ABI BRX;
- formato ELF e loader próprio;
- headers de sistema;
- libc BRX;
- syscalls BRX para arquivos, memória, processos, threads e rede;
- VFS, permissões e dispositivos;
- terminal, pipes e sinais;
- SDK de janela, input, áudio e gráficos;
- sistema de build e gerenciador de pacotes.

O compilador pode inicialmente rodar fora do BRX e produzir executáveis BRX. Em uma etapa posterior, o próprio compilador será recompilado para rodar dentro do BRX, criando um sistema self-hosting.

## Linguagens

A prioridade não é implementar uma linguagem nova dentro do kernel. É criar alvos BRX para linguagens maduras:

| Linguagem/ecossistema | Adaptação necessária |
|---|---|
| C | headers BRX, libc, linker e syscalls |
| C++ | runtime, threads, filesystem, exceções e ABI |
| Rust | target BRX, allocator, std/core e linker |
| Go | runtime de threads, rede, arquivos e relógio |
| Python | port do interpretador, filesystem, sockets e extensão nativa |
| Lua | port simples sobre a libc/SDK BRX |
| JavaScript | port do runtime e integração de I/O |
| C#/.NET | runtime, JIT/AOT e APIs de sistema |

## Ferramentas Linux

A adaptação source-first é preferível para Git, CMake, Ninja, GCC, LLVM, Python, Lua e ferramentas semelhantes. Em vez de fazer o BRX fingir ser Linux inteiro, o código é recompilado contra:

- `brx-libc`;
- `brx-posix`;
- `brx-vfs`;
- `brx-net`;
- `brx-thread`;
- `brx-terminal`.

Uma camada opcional de compatibilidade Linux pode ser adicionada depois para reduzir o trabalho de ports, mas não é a identidade do BRX.

## Jogos

Para jogos, o caminho é portar o engine ou a camada de plataforma, não traduzir cada instrução do jogo:

```text
Engine ou código do jogo
        |
Renderer BRX / Vulkan BRX / OpenGL BRX
Input BRX / áudio BRX / filesystem BRX
        |
SDK BRX
        |
Drivers GPU, áudio, teclado, mouse, USB e rede
```

Engines como SDL, GLFW, Godot ou partes de engines próprias podem receber backends BRX. O jogo então é compilado para BRX. Isso exige drivers gráficos e de áudio reais; por isso a compatibilidade de jogos depende do avanço da camada de hardware.

## Relação com AnyPS5

AnyPS5 é uma referência útil para a ideia de reimplementar interfaces e bibliotecas sem emulação tradicional. Porém, para o BRX, a estratégia principal será ainda mais simples quando houver código-fonte disponível: portar a biblioteca/engine e recompilar para BRX. Relinking e tradução binária permanecem como recursos secundários para software que não possa ser recompilado.

## Ordem prática

1. estabilizar kernel, memória, processos e VFS;
2. definir ABI e syscalls BRX;
3. criar headers, libc mínima e linker target;
4. portar shell, CMake/Ninja e utilitários básicos;
5. portar Git e bibliotecas de rede/TLS;
6. portar Python, Lua e outras linguagens;
7. criar SDK gráfico, input e áudio;
8. portar engines e jogos de código disponível;
9. adicionar compatibilidade binária Linux somente onde for realmente necessário.
