#!/usr/bin/env python3
"""
Converte binário para formato de ROM do Logisim Evolution
Gera arquivo com header "v2.0 raw" seguido de palavras de 32 bits em hex.

Formato de saída:
v2.0 raw
00000297
00010113
00000517
...

Uso: python3 bin_to_rom.py main.bin > rom.txt
"""

import sys
import struct
import os


def main():
    if len(sys.argv) != 2:
        print("❌ Uso: bin_to_rom.py <arquivo_binario>", file=sys.stderr)
        print("   Exemplo: python3 bin_to_rom.py main.bin > rom.txt", file=sys.stderr)
        sys.exit(2)

    infile = sys.argv[1]

    if not os.path.isfile(infile):
        print(f"❌ Arquivo não encontrado: {infile}", file=sys.stderr)
        sys.exit(2)

    # Lê o arquivo binário
    with open(infile, "rb") as f:
        data = f.read()

    if len(data) == 0:
        print("⚠️  Aviso: Arquivo binário vazio!", file=sys.stderr)
        print("v2.0 raw")
        return

    # Preenche com zeros até múltiplo de 4 bytes
    pad = (-len(data)) % 4
    if pad:
        data += b"\x00" * pad

    # Header do Logisim Evolution
    print("v2.0 raw")

    # Converte para palavras de 32 bits (little-endian)
    num_words = len(data) // 4
    for i in range(0, len(data), 4):
        word = struct.unpack_from("<I", data, i)[0]
        print(f"{word:08x}")

    # Estatísticas para stderr (não vai pro arquivo de saída)
    # print(f"✅ Convertido: {num_words} palavras ({len(data)} bytes)", file=sys.stderr)
    if pad:
        print(f"   Padding: {pad} byte(s) adicionado(s)", file=sys.stderr)


if __name__ == "__main__":
    main()
