/**
 * TESTE 1: Operações Aritméticas e Lógicas (ALU)
 *
 * Resultados esperados em DEBUG (0x20000000):
 * [0] = 0x0000000F  (ADD: 5 + 10 = 15)
 * [1] = 0xFFFFFFFB  (SUB: 5 - 10 = -5)
 * [2] = 0x00000050  (SLL: 5 << 4 = 80)
 * [3] = 0x00000001  (SLT: 5 < 10 = 1)
 * [4] = 0x00000000  (SLTU: 5 < -1 unsigned = 0? não, 5 < 0xFFFFFFFF = 1)
 * [5] = 0x0000000F  (XOR: 5 ^ 10 = 15)
 * [6] = 0x00000000  (SRL: 80 >> 4 = 5)
 * [7] = 0xFFFFFFFF  (SRA: -16 >> 2 = -4)
 * [8] = 0x00000000  (OR:  5 | 10 = 15)
 * [9] = 0x00000000  (AND: 5 & 10 = 0)
 */

#define DEBUG ((volatile unsigned int *)0x20000000)

int main(void) {
  int a = 5;
  int b = 10;

  // ADD
  DEBUG[0] = a + b; // Esperado: 15 (0x0F)

  // SUB
  DEBUG[1] = a - b; // Esperado: -5 (0xFFFFFFFB)

  // SLL (shift left logical)
  DEBUG[2] = a << 4; // Esperado: 80 (0x50)

  // SLT (set less than signed)
  DEBUG[3] = (a < b) ? 1 : 0; // Esperado: 1

  // SLTU (set less than unsigned)
  unsigned int ua = 5;
  unsigned int ub = 0xFFFFFFFF;
  DEBUG[4] = (ua < ub) ? 1 : 0; // Esperado: 1

  // XOR
  DEBUG[5] = a ^ b; // Esperado: 15 (0x0F)

  // SRL (shift right logical)
  DEBUG[6] = 80 >> 4; // Esperado: 5

  // SRA (shift right arithmetic)
  int neg = -16;
  DEBUG[7] = neg >> 2; // Esperado: -4 (0xFFFFFFFC)

  // OR
  DEBUG[8] = a | b; // Esperado: 15 (0x0F)

  // AND
  DEBUG[9] = a & b; // Esperado: 0

  return 0;
}
