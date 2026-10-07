# Dimensionamento real do BRX

## Conclusão executiva

**Bootar não é ser um sistema operacional.** Um bootstrap que imprime uma mensagem pode ter milhares de linhas; um kernel funcional precisa de processos, memória virtual, interrupções, syscalls, armazenamento e drivers; um sistema utilizável ainda precisa de init, shell, bibliotecas, comandos, testes, instalação e documentação.

A pesquisa paralela comparou dez projetos em snapshots oficiais. As métricas não são diretamente somáveis: alguns projetos contam blobs, outros linhas físicas, alguns incluem user space, testes, firmware e código importado. A regra correta é comparar **escopos equivalentes**.

A recomendação para o BRX é começar com uma arquitetura e uma plataforma reproduzível, construir um sistema CLI próprio e só depois ampliar compatibilidade source-first.

## Comparação resumida

| Projeto | Escopo medido | Tamanho observado | O que isso ensina |
|---|---|---:|---|
| xv6 RISC-V | Kernel + user space + mkfs, apenas C/H/assembly selecionados | 12.435 linhas físicas; 266.892 bytes | Um sistema didático completo pode ser pequeno, mas não tem a abrangência de um sistema geral. |
| MINIX 3 | Checkout completo | 71.064 arquivos; aproximadamente 534,3 MB; 9,85 milhões de linhas físicas filtradas | Microkernel pequeno não representa drivers, servidores, bibliotecas e user space. |
| Redox | 11 repositórios centrais, sem todo o ecossistema | 6.864 arquivos; 398.346 linhas físicas; 16,1 MB | Kernel, libc, drivers, serviços, shell, testes e build ficam distribuídos. |
| Managarm | Repositório central | 225.398 linhas físicas; 6,78 MB; 1.149 arquivos textuais | Kernel, pré-kernel, drivers e serviços formam blocos diferentes. |
| SerenityOS | Checkout completo | 18.795 blobs; 115,8 MB | Kernel: 5,5 MB; userland: 41,4 MB; testes: 33,5 MB. |
| Haiku | Seleção ampla de fontes próprias e extensões | 4,57 milhões de linhas físicas; 134,3 MB | Drivers, filesystem, apps e servidores são muito maiores que o núcleo isolado. |
| Linux | Árvore completa | 96.055 caminhos rastreados | Inclui drivers, arquiteturas, docs, ferramentas e testes; não são 96 mil arquivos de kernel puro. |
| FreeBSD | `src` completo | 114.648 blobs; aproximadamente 1,54 GB | `sys/` sozinho tem aproximadamente 546 MB e já inclui kernel, drivers e muitas plataformas. |
| OpenBSD | `src` completo | 87.038 arquivos; aproximadamente 1,54 GB | `sys/` tem aproximadamente 614 MB; o restante inclui base, regressões e userland. |
| NetBSD | `src` completo | 206.513 blobs; aproximadamente 3,39 GB | `sys/` tem aproximadamente 531 MB; `external/` concentra grande volume importado. |

## Tamanho recomendado para o BRX

As faixas abaixo são **metas de planejamento**, não uma alegação de que o BRX já possui esse tamanho ou funcionalidade.

| Etapa | Meta | Faixa inicial de código próprio | Critério real de conclusão |
|---|---|---:|---|
| A | Bootstrap verificável | 1–5 mil linhas; 10–50 arquivos; 50–250 kB | Carregar o kernel, inicializar serial, registrar mapa de memória e tratar falha. |
| B | Kernel funcional de uma plataforma | 15–50 mil linhas; 100–400 arquivos; 0,5–2 MB | Processos isolados, memória virtual, syscalls, scheduler, console e armazenamento testados. |
| C | Sistema CLI utilizável | 60–200 mil linhas; 500–2.000 arquivos; 3–20 MB | Init, shell, libc/runtime, filesystem, comandos, instalação e execução de programas. |
| D1 | Compatibilidade source-first delimitada | +20–100 mil linhas; 200–1.200 arquivos; 2–10 MB | 20–50 programas portados e testados, com APIs e patches próprios documentados. |
| D2 | Runtime/ABI próprio estável | +30–150 mil linhas; 300–1.500 arquivos; 3–15 MB | ABI documentada, libc/runtime estável, testes de regressão e compatibilidade entre releases. |

## O que deve ser contado separadamente

A partir deste ponto, o BRX deve publicar quatro métricas por commit:

1. **Bootstrap/pré-kernel** — código que prepara CPU, memória e entrada do kernel.
2. **Kernel e drivers próprios** — memória, processos, scheduler, VFS, interrupções e hardware suportado.
3. **User space e runtime próprios** — init, shell, libc, serviços, ferramentas e APIs.
4. **Testes, documentação, toolchain, ports e código externo** — separados entre código local e fontes baixadas/vendor.

Não devemos usar tamanho do checkout como sinônimo de tamanho do kernel, nem usar número de arquivos como sinônimo de maturidade.

## Implicação para a arquitetura do BRX

O BRX atual já tem uma fundação real, mas ainda está na transição entre a etapa A e a etapa B:

- boot e saída serial testados;
- alocador físico inicial;
- IDT/PIC/PIT;
- enumeração PCI;
- teclado e mouse PS/2;
- parser ELF;
- parser PE próprio;
- regiões de memória virtual;
- tabela inicial de processos;
- contrato de boot BRX;
- testes host e boot QEMU.

Ainda não é correto chamar o BRX de sistema utilizável. Faltam, entre outros:

- bootstrap BRX próprio substituindo o carregador temporário;
- tabelas de páginas reais;
- troca de contexto;
- entrada de syscalls;
- VFS e filesystem persistente;
- driver de armazenamento;
- init e user space;
- shell e libc;
- rede e toolchain nativo;
- runtime de compatibilidade próprio.

## Compatibilidade source-first

Portar código-fonte não significa executar binários de outro sistema. O BRX deve:

1. definir APIs e ABI próprias;
2. criar headers e bibliotecas BRX;
3. portar programas selecionados a partir de fontes, respeitando suas licenças;
4. manter patches próprios e rastreáveis;
5. testar cada programa no BRX;
6. não declarar compatibilidade geral com base apenas em um parser PE/ELF.

A camada de compatibilidade deve ser orçada separadamente do kernel. Os ports e as fontes baixadas também não devem ser contados como código próprio do BRX.

## Fontes primárias consultadas

- [Linux](https://github.com/torvalds/linux)
- [FreeBSD](https://github.com/freebsd/freebsd-src)
- [OpenBSD](https://github.com/openbsd/src)
- [NetBSD](https://github.com/NetBSD/src)
- [Haiku](https://github.com/haiku/haiku)
- [SerenityOS](https://github.com/SerenityOS/serenity)
- [Redox OS](https://gitlab.redox-os.org/redox-os/redox)
- [MINIX 3](https://github.com/Stichting-MINIX-Research-Foundation/minix)
- [xv6 RISC-V](https://github.com/mit-pdos/xv6-riscv)
- [Managarm](https://github.com/managarm/managarm)

As medições foram feitas em snapshots específicos e com regras diferentes por projeto. Elas servem para dimensionamento e comparação de escopo, não para afirmar equivalência de qualidade, cobertura ou maturidade.
