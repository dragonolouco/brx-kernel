# Política clean-room do BRX

## Regra principal

O BRX será implementado do zero. Projetos existentes podem ser estudados para compreender formatos, contratos, algoritmos publicados, requisitos de hardware e comportamento observável, mas o código-fonte original não será copiado, incorporado, traduzido linha a linha ou usado como base de implementação.

A meta é obter interoperabilidade, não duplicar a implementação de outro projeto.

## O que pode ser estudado

- especificações públicas de ELF, PE/COFF, Vulkan, USB, PCI e outros formatos;
- documentação de APIs e ABI;
- manuais de fabricantes e protocolos publicados;
- artigos técnicos e livros;
- comportamento observável em testes controlados;
- formatos de arquivo e assinaturas públicas;
- resultados de compatibilidade e casos de erro;
- arquitetura geral de sistemas conhecidos, sem copiar sua implementação.

## O que não será usado

- código-fonte do kernel Linux como base do kernel BRX;
- executáveis pré-compilados de Proton, Wine ou emuladores;
- cópia de funções, arquivos, nomes internos ou estruturas proprietárias;
- tradução automática do código original para C/C++ do BRX;
- desmontagem com reaproveitamento de implementação;
- remoção de licenças ou atribuições de projetos estudados.

## Processo de implementação

Cada componente BRX deve seguir este ciclo:

1. definir o comportamento que o BRX precisa oferecer;
2. consultar especificações públicas e documentar o contrato;
3. escrever uma especificação BRX própria;
4. implementar do zero com tipos, nomes e arquitetura BRX;
5. criar testes de conformidade e casos de erro;
6. testar com dados e programas permitidos;
7. registrar limitações e diferenças do BRX.

Quando uma implementação externa for consultada durante a pesquisa, ela serve apenas para entendimento. O código BRX deve ser escrito de forma independente, por pessoas que trabalham a partir da especificação BRX e dos contratos públicos.

## Emulação e compatibilidade

O BRX não precisa copiar um emulador para oferecer compatibilidade. Pode implementar uma camada própria que:

- reconhece o formato do arquivo;
- valida headers e dependências;
- traduz chamadas para APIs BRX;
- recompõe comandos gráficos para o backend BRX;
- converte eventos de teclado, mouse, controle e áudio;
- mantém memória, threads, sinais e arquivos segundo os contratos definidos;
- apresenta tudo como uma função integrada de abertura do BRX.

Esse componente será um **runtime de compatibilidade BRX**, escrito do zero. O usuário não abrirá um programa separado.

## Limite legal

Clean-room e interoperabilidade são princípios de engenharia, não uma garantia jurídica universal. Licenças, direitos autorais, patentes, marcas, termos de uso e jurisdição precisam ser avaliados antes de distribuir componentes. O projeto manterá atribuição às referências conceituais e não usará nomes de terceiros para sugerir endosso.
