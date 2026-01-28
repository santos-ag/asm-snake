#!/usr/bin/env python3
"""
Converte binário em palavras de 32 bits (little-endian) para um arquivo de ROM
com uma palavra por linha em hex (8 dígitos, sem 0x). Ex:
00000000
00000001
00000001
00000002
...
Uso: python3 bin_to_rom.py main.bin > rom.txt
"""

import sys
import struct
import os

if len(sys.argv) != 2:
    print("Uso: bin_to_rom.py <arquivo_binario>", file=sys.stderr)
    sys.exit(2)

infile = sys.argv[1]
if not os.path.isfile(infile):
    print(f"Arquivo não encontrado: {infile}", file=sys.stderr)
    sys.exit(2)

data = open(infile, "rb").read()
# Preenchimento para múltiplo de 4 bytes
pad = (-len(data)) % 4
if pad:
    data += b"\x00" * pad

out_lines = []
for i in range(0, len(data), 4):
    word = struct.unpack_from("<I", data, i)[0]  # little-endian
    out_lines.append(f"{word:08x}")

sys.stdout.write("\n".join(out_lines))
