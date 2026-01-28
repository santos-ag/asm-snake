#!/bin/bash
set -euo pipefail

OBJDUMP="$(command -v riscv-none-elf-objdump || true)"
if [ -z "$OBJDUMP" ]; then
  echo "Erro: riscv-none-elf-objdump não encontrado no PATH" >&2
  exit 1
fi

if [ ! -f main.elf ]; then
  echo "Erro: main.elf não encontrado no diretório atual" >&2
  exit 1
fi

OUTFILE="main.dssbly"

# Tenta ver se o objdump suporta 'reg-names'
if $OBJDUMP -M help 2>&1 | grep -qi 'reg-names'; then
  echo "Usando reg-names=abi para nomes de registradores (ABI)..."
  $OBJDUMP -d -M reg-names=abi main.elf > "$OUTFILE"
else
  echo "Opção 'reg-names' não suportada; fazendo disassemble sem modificador..."
  $OBJDUMP -d main.elf > "$OUTFILE"
fi

echo "Saída escrita em $OUTFILE"

