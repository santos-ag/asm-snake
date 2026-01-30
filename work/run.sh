#!/bin/bash
set -e

echo "🛠️  Compilando para RV32I..."

# Limpa builds anteriores
rm -f main.elf main.bin rom.txt main.map main.dump

# Compila
riscv-none-elf-gcc \
  -march=rv32i -mabi=ilp32 \
  -O0 -g \
  -nostdlib -nostartfiles \
  -ffreestanding \
  -T linker.ld \
  -Wl,-Map=main.map \
  crt0.S main.c \
  -o main.elf

echo "✅ ELF gerado: main.elf"

# Gera dump para debug
riscv-none-elf-objdump -d main.elf >main.dump
echo "✅ Disassembly: main.dump"

# Mostra informações do ELF
echo ""
echo "📊 Seções do ELF:"
riscv-none-elf-size main.elf
echo ""

# Extrai binário
riscv-none-elf-objcopy -O binary main.elf main.bin
echo "✅ BIN gerado: main.bin ($(stat -f%z main.bin 2>/dev/null || stat -c%s main.bin) bytes)"

# Converte para ROM do Logisim
python3 bin_to_rom.py main.bin >rom.txt 2>&1

echo "✅ ROM pronta: rom.txt"
echo ""
echo "🎯 Próximos passos:"
echo "   1. Abra rom.txt e copie o conteúdo"
echo "   2. Cole na ROM do Logisim"
echo "   3. Configure PC inicial = 0x00000000"
echo "   4. Execute e observe os endereços:"
echo "      - 0x100: valor 10 (0x0000000A)"
echo "      - 0x104: valor 20 (0x00000014)"
echo "      - 0x108: valor 150 (0x00000096)"
echo "      - 0x10C: valor 5 (0x00000005)"
