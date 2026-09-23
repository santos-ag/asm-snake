# Snake Game — RISC‑V RV32I

Uma implementação do clássico jogo da cobrinha (Snake) desenvolvida para a disciplina de Arquitetura de Computadores, projetada para rodar em um processador RISC‑V RV32I dedicado implementado no Logisim Evolution. 

O projeto contém o código do jogo em C com startup em Assembly RV32I, linker script, gerador de ROM e o circuito esquemático completo integrado a um display RGB 16x16 e entrada de teclado via MMIO.

---

## Estrutura do Repositório

- `project.circ` — Circuito completo no Logisim Evolution (CPU RV32I + periféricos)
- `CU.ROM` — Arquivo de microcódigo/ROM da Unidade de Controle (Control Unit)
- `docker-compose.yml` — Configuração para build reproduzível em container
- `Dockerfile` — Imagem com toolchain RISC-V GCC pré-configurada
- `work/` — Código-fonte do jogo e scripts de compilação
  - `main.c` — Lógica do jogo, controle de movimento, colisões, RNG e drivers MMIO
  - `crt0.S` — Startup assembly (configuração de SP, limpeza do BSS e salto para main)
  - `linker.ld` — Linker script que posiciona `.text` em `0x00000000` (RAM de 64KB)
  - `run.sh` — Script automatizado de compilação, linkagem e conversão de ROM
  - `bin_to_rom.py` — Utilitário de conversão de binário bruto para formato `v2.0 raw` do Logisim
  - `dssbly.sh` — Script auxiliar para desmonte (disassembly) do ELF via `objdump`
  - `rom.txt` — ROM pré-gerada pronta para execução
- `tests/` — Suíte de testes para validação do núcleo RV32I
  - `t0 - ram/` — Testes de barramento e leitura/escrita na RAM
  - `t1-operation/` — Validação das operações lógicas e aritméticas da ALU
  - `t2-branch/` — Testes de instruções de desvio condicional
  - `t3-c_test/` — Programa de testes em C com checagens de memória e algoritmos

---

## Arquitetura Alvo & Mapeamento MMIO

O jogo interage com o hardware através de registradores MMIO (*Memory-Mapped I/O*) mapeados em endereços fixos:

| Registrador | Endereço | Largura | Descrição |
|---|---|---|---|
| `REG_COR` | `0x200` | 32 bits | Cor do pixel em formato RGB (ex: `0x228B22` verde, `0xFF0000` vermelho) |
| `REG_X` | `0x204` | 8 bits | Coordenada X na grade (0 a 15) |
| `REG_Y` | `0x208` | 8 bits | Coordenada Y na grade (0 a 15) |
| `REG_RESET` | `0x20C` | 8 bits | Pulso de reset do display |
| `REG_CLOCK` | `0x210` | 8 bits | Pulso de clock (`0 -> 1 -> 0`) para efetivar a escrita do pixel |
| `REG_INPUT` | `0x214` | 8 bits | Leitura da tecla pressionada no teclado (ASCII 7 bits) |

O display matricial consiste em uma grade 16x16. A função `desenha(x, y, cor)` programa a cor, define as coordenadas X/Y e gera a borda de subida no registrador `REG_CLOCK` para atualizar o buffer de vídeo.

---

## Controles

O jogo lê os caracteres ASCII diretamente via `REG_INPUT` com detecção de borda (*edge detection*) para evitar passos múltiplos por tecla pressionada:

- `W` / `w` → Cima
- `S` / `s` → Baixo
- `A` / `a` → Esquerda
- `D` / `d` → Direita

---

## Compilação e Build

### Opção 1: Via Docker (Recomendado)

Não requer a instalação manual da toolchain RISC-V no computador hospedeiro:

```bash
# Executa o build dentro do container Docker
docker compose run --rm riscv ./run.sh
```

Os artefatos compilados (`main.elf`, `main.bin`, `rom.txt`, `main.dump` e `main.map`) serão gerados dentro da pasta `work/`.

### Opção 2: Localmente no Host

Requer `riscv-none-elf-gcc` e `python3` configurados no seu `PATH`:

```bash
cd work
./run.sh
```

---

## Como Executar no Logisim Evolution

1. Abra o **Logisim Evolution**.
2. Abra o arquivo `project.circ` na raiz deste repositório.
3. No painel lateral, localize o subcircuito **`InstMem`**.
4. Clique com o botão direito sobre o componente **ROM de Instruções** e selecione **Load Image...** (ou *Carregar Imagem*).
5. Selecione o arquivo compilado `work/rom.txt`.
6. Volte para o circuito principal **`main`**.
7. Inicie a simulação:
   - Ative o clock do simulador: menu **Simulate → Ticks Enabled** (ou pressione `Ctrl + K`).
   - Ajuste a frequência do clock em **Simulate → Tick Frequency** (recomenda-se entre 64 Hz e 512 Hz conforme a velocidade desejada).
8. Para controlar a cobrinha:
   - Clique na ferramenta de interação (*Hand Tool* / mãozinha no Logisim).
   - Clique sobre o componente de **Keyboard** para dar foco.
   - Use as teclas **W, A, S, D** para guiar a cobra pela tela.
