# BRX Compatibility Runtime

## Objetivo

O BRX terá uma abertura integrada de aplicativos. O usuário seleciona um arquivo e o sistema identifica seu formato, prepara internamente o código de compatibilidade compilado a partir das fontes adaptadas e inicia o programa sem exigir que o usuário conheça nomes como Wine, Proton ou um tradutor separado.

A experiência será semelhante a:

```text
abrir arquivo
    -> BRX identifica formato
    -> BRX escolhe loader/runtime
    -> tela nativa de carregamento
    -> aplicativo inicia
```

Internamente existe código real trabalhando, mas ele aparece para o usuário como uma função do próprio BRX. O usuário não abre nem vê um executável de tradução. O componente é carregado como parte do fluxo de abertura do sistema, da mesma forma que o Windows prepara serviços, bibliotecas e o processo de um aplicativo quando o usuário clica em seu ícone.

Isso não significa colocar bibliotecas grandes dentro do kernel. O código-fonte adaptado será compilado como componentes nativos do sistema e integrado ao loader, à sessão gráfica e aos serviços BRX. Para o usuário, a sequência será apenas: clicar, ver a animação de carregamento e receber a janela do aplicativo.

## Detecção automática

O detector deve usar conteúdo e metadados, não somente extensão do arquivo:

| Formato | Identificação inicial | Caminho BRX |
|---|---|---|
| BRX ELF | cabeçalho ELF + notas ABI BRX | loader nativo |
| ELF Linux | cabeçalho ELF + ABI/arquitetura | executar nativo se compatível ou camada Linux opcional |
| Windows PE | assinatura `MZ`, cabeçalho `PE\\0\\0`, arquitetura | runtime Windows baseado em código Wine/Proton adaptado |
| Script | primeira linha `#!` | intérprete BRX registrado |
| Pacote BRX | manifesto e assinatura do pacote | instalador/gerenciador BRX |
| Formato desconhecido | nenhuma assinatura válida | mensagem nativa de formato não suportado |

A detecção precisa validar arquitetura, permissões, dependências, integridade e compatibilidade do runtime antes de iniciar. Extensões como `.exe` ou `.sh` são apenas uma dica para a interface, nunca a prova do formato.

## Componentes do runtime

O projeto será dividido em módulos de user space:

- `brx-exec`: detector e dispatcher automático integrado ao shell;
- `brx-loader`: carregador ELF/PE e resolução de dependências integrado à abertura;
- `brx-runtime-windows`: APIs Windows reimplementadas sobre BRX;
- `brx-graphics`: D3D para Vulkan/BRX GPU;
- `brx-input`: teclado, mouse, USB, gamepads e controles;
- `brx-audio`: áudio BRX;
- `brx-prefix`: configuração isolada de cada aplicativo;
- `brx-session`: janela de carregamento, logs, crash report e encerramento;
- `brx-installer`: instalação de runtimes e componentes.

Esses componentes não serão um programa que o usuário precise abrir manualmente. O shell/desktop BRX usará as funções internas como parte do fluxo normal de abertura. Os nomes acima são divisões de código e responsabilidades do sistema, não aplicativos que aparecerão no menu.

## Uso do código do Proton

O Proton oficial é descrito como uma ferramenta para Steam Play baseada em Wine. O repositório contém Wine, DXVK, vkd3d-proton e outros componentes. Portanto, o BRX não deve copiar um executável pronto nem fingir que Proton é uma biblioteca única.

O caminho correto é:

1. estudar e selecionar os componentes de código-fonte necessários, sem incorporar executáveis prontos;
2. preservar os arquivos de licença, avisos e atribuições de cada componente;
3. criar uma adaptação BRX das dependências de Unix/Linux;
4. compilar os componentes para a ABI BRX e incorporá-los ao sistema;
5. integrar o dispatcher e a tela de carregamento ao shell do BRX, sem expor um aplicativo de tradução;
6. manter uma matriz de compatibilidade por aplicativo e API.

Proton tem licença BSD-3-Clause no repositório principal, Wine é LGPL-2.1 e DXVK usa licença zlib, enquanto outras dependências podem ter termos próprios. O nome do runtime BRX pode ser diferente, mas a origem e as licenças não podem ser apagadas.

## O que é e o que não é

É:

- uma camada integrada de execução;
- um detector automático de formatos;
- um conjunto de APIs reimplementadas;
- uma tradução gráfica e de sistema quando necessária;
- uma experiência de abertura direta para o usuário.

Não é:

- uma cópia do kernel Linux;
- um executável Proton escondido dentro do kernel;
- uma promessa de que qualquer programa roda sem suas dependências;
- um emulador de CPU quando o código já pode ser compilado para BRX;
- uma forma de remover licenças ou atribuições dos projetos usados.

## Prioridade do BRX

Quando houver código-fonte, o BRX prefere recompilar diretamente:

```text
código-fonte -> headers BRX -> libc/SDK BRX -> executável BRX nativo
```

O runtime inspirado em Proton será usado principalmente para aplicativos Windows cujo código não esteja disponível ou não possa ser portado. Para ferramentas abertas como Git, CMake, Python e muitos utilitários, o objetivo é um port source-first nativo.
