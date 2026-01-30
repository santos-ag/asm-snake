#!/bin/bash
set -e

# Diretório onde está este script (work/tests)
TESTS_DIR="$(cd "$(dirname "$0")" && pwd)"
# Diretório work (um nível acima)
WORK_DIR="$(dirname "$TESTS_DIR")"

echo "╔════════════════════════════════════════════╗"
echo "║   BATERIA DE TESTES RISC-V RV32I           ║"
echo "╚════════════════════════════════════════════╝"
echo

# Lista de testes
TESTS=(
    "test_alu:Operações ALU"
    "test_branch:Instruções Branch"
    "test_memory:Load/Store"
    "test_jump:JAL/JALR"
    "test_immediate:LUI/AUIPC"
    "test_all:Teste Completo"
)

# Cria diretório de saída
mkdir -p "$TESTS_DIR/output"

# Detecta objdump
OBJDUMP="$(command -v riscv-none-elf-objdump || true)"
if [ -z "$OBJDUMP" ]; then
    echo "Erro: riscv-none-elf-objdump não encontrado no PATH" >&2
    exit 1
fi

# Verifica suporte a reg-names
USE_ABI_NAMES=false
if $OBJDUMP -M help 2>&1 | grep -qi 'reg-names'; then
    USE_ABI_NAMES=true
    echo "ℹ️  Usando reg-names=abi para nomes de registradores"
    echo
fi

for test_entry in "${TESTS[@]}"; do
    test_name="${test_entry%%:*}"
    test_desc="${test_entry##*:}"

    # Verifica se o arquivo de teste existe
    if [ ! -f "$TESTS_DIR/${test_name}.c" ]; then
        echo "⚠️  Pulando: ${test_name}.c não encontrado"
        continue
    fi

    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "🧪 Compilando: $test_desc ($test_name.c)"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    # Arquivos temporários
    ELF_FILE="$TESTS_DIR/output/${test_name}.elf"
    BIN_FILE="$TESTS_DIR/output/${test_name}.bin"

    # Arquivos finais (mantidos)
    ROM_FILE="$TESTS_DIR/output/${test_name}_rom.txt"
    DSSBLY_FILE="$TESTS_DIR/output/${test_name}.dssbly"

    # Compila (crt0.S e linker.ld estão em work/)
    riscv-none-elf-gcc \
        -march=rv32i -mabi=ilp32 \
        -O0 \
        -nostdlib -nostartfiles \
        -ffreestanding \
        -T "$WORK_DIR/linker.ld" \
        "$WORK_DIR/crt0.S" "$TESTS_DIR/${test_name}.c" \
        -o "$ELF_FILE"

    # Gera binário
    riscv-none-elf-objcopy -O binary "$ELF_FILE" "$BIN_FILE"

    # Converte para ROM Logisim (arquivo final .txt)
    python3 "$WORK_DIR/bin_to_rom.py" "$BIN_FILE" >"$ROM_FILE"

    # Gera disassembly com nomes ABI (arquivo final .dssbly)
    if [ "$USE_ABI_NAMES" = true ]; then
        $OBJDUMP -d -M reg-names=abi "$ELF_FILE" >"$DSSBLY_FILE"
    else
        $OBJDUMP -d "$ELF_FILE" >"$DSSBLY_FILE"
    fi

    # Remove arquivos intermediários (.elf e .bin)
    rm -f "$ELF_FILE" "$BIN_FILE"

    echo "✅ ${test_name}_rom.txt"
    echo "✅ ${test_name}.dssbly"
    echo
done

echo "╔════════════════════════════════════════════╗"
echo "║   TODOS OS TESTES COMPILADOS!              ║"
echo "╚═════════════════════���══════════════════════╝"
echo
echo "📁 Arquivos em: $TESTS_DIR/output/"
echo
echo "   Mantidos:"
echo "   ├── *_rom.txt   (hex para Logisim)"
echo "   └── *.dssbly    (disassembly com nomes ABI)"
echo
