# Snake Game — RISC‑V RV32I

Uma implementação do jogo da cobrinha (Snake) para a diciplina de arquitetura de computadores desenvolvida para rodar em um processador RISC‑V RV32I caseiro. O projeto contém o código em C + startup em assembly, script de link, utilitários para gerar ROM compatível com Logisim Evolution e uma suíte de testes para plataformas RV32I.

---

## Visão geral

O jogo é implementado em C, com um pequeno startup em RISC‑V assembly (crt0.S) e um script de link (work/linker.ld) que posiciona o .text em 0x00000000. A saída do processo de build é:

- `main.elf` — ELF gerado
- `main.bin` — "raw binary" extraído do ELF
- `rom.txt` — ROM no formato "v2.0 raw" (palavras de 32 bits em hex) pronta para carregar no Logisim Evolution

O jogo interage com o "hardware" (display simples e entrada) através de registradores MMIO mapeados em endereços fixos.

---

## Arquitetura alvo (hardware / MMIO)

Registradores MMIO usados pelo jogo (do código `work/main.c`):

```c
#define REG_COR (*(volatile unsigned int *)0x200)
#define REG_X (*(volatile unsigned char *)0x204)
#define REG_Y (*(volatile unsigned char *)0x208)
#define REG_RESET (*(volatile unsigned char *)0x20C)
#define REG_CLOCK (*(volatile unsigned char *)0x210)
#define REG_INPUT (*(volatile unsigned char *)0x214)
```

- REG_COR (0x200): cor da pixel/escrita (32 bits)
- REG_X  (0x204): coord X (8 bits)
- REG_Y  (0x208): coord Y (8 bits)
- REG_RESET (0x20C): sinal de reset do periférico (8 bits)
- REG_CLOCK (0x210): pulso para confirmar escrita (edge)
- REG_INPUT (0x214): leitura de teclado (ascii/scan code)

O display implementado espera coordenadas X/Y entre 0 e 15 (tela 16x16). As funcoes de desenho escrevem cor, X, Y, pulsam CLOCK para efetivar o pixel.

---

## Requisitos

- Toolchain RISC‑V: riscv-none-elf-gcc, riscv-none-elf-objcopy, riscv-none-elf-objdump, riscv-none-elf-size, riscv-none-elf-nm
  - Ex.: xpack riscv-none-elf-gcc (a imagem Docker do repositório já baixa uma toolchain)
- Python 3 (para bin_to_rom.py)
- Logisim Evolution (ou outro simulador capaz de importar ROM no formato "v2.0 raw" com palavras de 32 bits)
- Docker para build reproduzível

---

## Compilar localmente (host)

Este repositório já inclui um script para compilar tudo: `work/run.sh`. Exemplo (no diretório root do repositório):

1. Certifique‑se de que `riscv-none-elf-gcc` está no PATH.
2. Execute:

```bash
cd work
./run.sh
```

O script:
- compila `crt0.S` e `main.c`
- linka usando `work/linker.ld`
- gera `main.elf`, `main.bin`, `main.dump` (disassembly)
- converte `main.bin` para `rom.txt` usando `work/bin_to_rom.py`

(Se preferir, abra `work/run.sh` para ver flags e passos exatos.)

---

## Compilar dentro do Docker

O repositório contém um `Dockerfile` que prepara uma imagem com a toolchain RISC‑V. Exemplo de uso:

```bash
# Construir a imagem (da raiz do repositório)
docker build -t snake-rv32i .

# Subir um container interativo com /work mapeado
docker run --rm -it -v "$(pwd)/work:/work" snake-rv32i

# Dentro do container:
cd /work
./run.sh
```

Isso garante que a versão da toolchain corresponda à usada para desenvolvimento.

---

## Gerar ROM e carregar no Logisim

Após o `./run.sh` você terá `work/rom.txt`. Instruções básicas no Logisim:

1. No Logisim Evolution, crie/abra o design do seu processador.
2. Adicione um componente ROM e carregue `work/rom.txt`.
3. Configure o PC inicial em `0x00000000`.
4. Execute (step/run) o processador.
5. Observe o periférico de vídeo: ele deve desenhar a grade 16x16 com a cobrinha e a fruta.

O arquivo `work/rom.txt` está no formato "v2.0 raw" (cada linha = 32 bits em hex), compatível com Logisim Evolution.

---

## Controles

O jogo usa teclas WASD (maiúsculas ou minúsculas) lidas via `REG_INPUT`. Mapeamento no código:

- W / w → cima
- S / s → baixo
- A / a → esquerda
- D / d → direita

Detecção de tecla feita por leitura de `REG_INPUT` e detecção de alteração (edge detection — só processa quando muda e não é zero) para evitar múltiplos passos por leitura.

---

## Mapa de memória e layout

O linker script define RAM em 0x00000000 com 64KB (0x10000). O projeto também inclui uma suite de testes que documenta áreas usadas na RAM — veja `tests/t3-c_test/test.c` para um mapa detalhado de regiões utilizadas (resultados, debug, arrays, relatório). Exemplo (da suite de testes):

- 0x100 - 0x1FF: resultados dos testes
- 0x200 - 0x2FF: dados de debug
- 0x300 - 0x3FF: arrays de teste
- 0x400 - 0x4FF: relatório final

> Nota: Estes endereços são relativos ao uso da suite de testes; o jogo usa MMIO em 0x200+ e memória global para arrays de posição da cobrinha.

---

## Estrutura do projeto / arquivos importantes

- work/
  - main.c — código do jogo (desenho, lógica da cobrinha, RNG, I/O MMIO)
  - crt0.S — startup assembly (inicializa SP, limpa BSS, chama main)
  - linker.ld — script de linker (define ORIGIN = 0x00000000, RAM = 64KB)
  - run.sh — script de build completo (compilação, link, objcopy, bin_to_rom)
  - bin_to_rom.py — conversor de main.bin para `v2.0 raw` (Logisim)
  - rom.txt — ROM gerada (exemplo)
  - dssbly.sh — script para gerar disassembly com objdump
- Dockerfile — container com toolchain RISC‑V
- tests/ — suíte de testes RV32I (testes ALU, memória, branches, etc.)

---

## Testes

A pasta `tests/` contém uma suíte de testes em C projetada para rodar em um núcleo RV32I. Ela verifica operações ALU, memória, branches, chamadas de função e algoritmos (ex.: Fibonacci). Para usar/reusar os testes:

- Compile o `tests/t3-c_test/test.c` com o mesmo script/linker (ajustar `main.c` por `test.c` ou adaptar o processo de build).
- Carregue a ROM no simulador e verifique as regiões de memória reservadas para os resultados.

---

## Troubleshooting / Dicas

- "riscv-none-elf-gcc not found": instale a toolchain (xpack, riscv-gnu-toolchain, ou use o Dockerfile).
- Se o jogo não aparece no Logisim:
  - Verifique que `rom.txt` foi carregado corretamente.
  - Certifique‑se de que PC inicial foi configurado para 0x00000000.
  - Verifique se os sinais do periférico (CLOCK, RESET) estão conectados corretamente no design.
- Se o binário for muito grande, o `linker.ld` contém ASSERTs que falharão: verifique espaço para BSS/heap/stack.
- Para inspecionar o ELF: `riscv-none-elf-objdump -d main.elf` e `riscv-none-elf-size main.elf`.

---

## Como portar para hardware real (FPGA, SoC)

- O MMIO usado é simples (endereços fixos e writes por registrador). Garanta que:
  - Cada acesso a REG_COR, REG_X, REG_Y, REG_CLOCK seja mapeado para registers do periférico no seu barramento.
  - Considere sincronização/handshake se seu periférico for síncrono a outro clock.
- O arquivo `rom.txt` contém o binário em palavras de 32 bits. Em uma FPGA, converta para o formato de memória ROM/BRAM esperado.
- Se usar BRAM com endianidade diferente, cheque o `bin_to_rom.py` para adaptar ordem de bytes.
- Para portar, verifique as restrições de alinhamento do linker (TEXTO em 0x0) e que sua BRAM/ROM esteja inicializada nesse endereço.

---

## Referências rápidas (comandos)

Compilar (local):
```bash
cd work
docker compose run --rm riscv
./run.sh
# saída: main.elf main.bin rom.txt main.dump main.map
```

