# bin_to_rom.py
import sys

if len(sys.argv) != 2:
    print("uso: bin_to_rom.py main.bin")
    sys.exit(1)

data = open(sys.argv[1], "rb").read()

# padding para múltiplo de 4 bytes
if len(data) % 4:
    data += b"\x00" * (4 - len(data) % 4)

words = [int.from_bytes(data[i : i + 4], "little") for i in range(0, len(data), 4)]

# imprime 8 palavras por linha (padrão humano)
for i, w in enumerate(words):
    if i % 8 == 0:
        if i != 0:
            print()
    print(f"{w:08x}", end=" ")

print()
