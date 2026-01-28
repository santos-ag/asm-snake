#!/bin/bash
set -e

echo "🛠️  Compilando para RV32I..."

# limpa builds anteriores
rm -f main.elf main.bin rom.txt

# compila
riscv-none-elf-gcc \
  -march=rv32i -mabi=ilp32 \
  -O0 \
  -nostdlib -nostartfiles \
  -ffreestanding \
  -T linker.ld \
  crt0.S main.c \
  -o main.elf

echo "✅ ELF gerado: main.elf"

# extrai binário cru
riscv-none-elf-objcopy -O binary main.elf main.bin

echo "✅ BIN gerado: main.bin"

# converte para palavras hex (ROM Logisim)
python3 bin_to_rom.py main.bin > rom.txt

echo "✅ ROM pronta: rom.txt"
echo
echo "➡️  Agora é só copiar o conteúdo de rom.txt e colar no ROM do Logisim"
