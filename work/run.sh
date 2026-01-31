#!/bin/bash
set -e

echo "========================================"
echo "  Compilando para RV32I (RAM: 64KB)"
echo "========================================"

# Limpa builds anteriores
rm -f *.o main.elf main.bin rom.txt main.map main.dump

# Verifica se o compilador existe
if ! command -v riscv-none-elf-gcc &>/dev/null; then
  echo "❌ Erro: riscv-none-elf-gcc não encontrado!"
  echo "   Instale com: brew install riscv-gnu-toolchain"
  exit 1
fi

echo ""
echo "[1/5] Compilando crt0.S..."
riscv-none-elf-gcc \
  -march=rv32i \
  -mabi=ilp32 \
  -c crt0.S \
  -o crt0.o

echo "[2/5] Compilando main.c..."
riscv-none-elf-gcc \
  -march=rv32i \
  -mabi=ilp32 \
  -O0 \
  -g \
  -fno-builtin \
  -fno-stack-protector \
  -fno-pic \
  -fno-plt \
  -ffreestanding \
  -nostdlib \
  -c main.c \
  -o main.o

echo "[3/5] Linkando..."
riscv-none-elf-gcc \
  -march=rv32i \
  -mabi=ilp32 \
  -nostdlib \
  -nostartfiles \
  -ffreestanding \
  -T linker.ld \
  -Wl,-Map=main.map \
  -Wl,--no-relax \
  crt0.o main.o \
  -o main.elf

echo "[4/5] Gerando disassembly..."
riscv-none-elf-objdump -d -M no-aliases main.elf >main.dump

# Mostra informações
echo ""
echo "📊 Seções do ELF:"
riscv-none-elf-size main.elf

echo ""
echo "📍 Símbolos importantes:"
riscv-none-elf-nm main.elf | grep -E "_start|main|_bss|_text" | sort || true

# Extrai binário
echo ""
echo "[5/5] Gerando binário e ROM..."
riscv-none-elf-objcopy -O binary main.elf main.bin

BINSIZE=$(wc -c <main.bin | tr -d ' ')
echo "📏 Tamanho do binário: $BINSIZE bytes"

# Converte para formato Logisim
python3 bin_to_rom.py main.bin >rom.txt

echo ""
echo "========================================"
echo "  ✅ COMPILAÇÃO CONCLUÍDA!"
echo "========================================"

echo ""
echo "📄 Prévia do código (primeiras 16 instruções):"
echo "----------------------------------------"
head -17 rom.txt
echo "   ..."

echo ""
echo "📋 Primeiras instruções do disassembly:"
echo "----------------------------------------"
head -30 main.dump | tail -20

echo ""
echo "========================================"
echo "  🎯 PRÓXIMOS PASSOS NO LOGISIM"
echo "========================================"
echo ""
echo "1. Carregue rom.txt na ROM"
echo "2. Configure PC inicial = 0x00000000"
echo "3. Execute o processador"
echo ""
echo "4. Verifique RAM após execução:"
echo "   Word 0x40 (byte 0x100) = 0x0000000A (10)"
echo "   Word 0x41 (byte 0x104) = 0x00000014 (20)"
echo "   Word 0x42 (byte 0x108) = 0x00000096 (150)"
echo "   Word 0x43 (byte 0x10C) = 0x00000005 (5)"
echo ""
