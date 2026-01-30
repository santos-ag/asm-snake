/**
 * TESTE 2: Instruções de Branch
 *
 * Resultados esperados em DEBUG (0x20000000):
 * [0] = 0x00000001  (BEQ funcionou)
 * [1] = 0x00000002  (BNE funcionou)
 * [2] = 0x00000003  (BLT funcionou)
 * [3] = 0x00000004  (BGE funcionou)
 * [4] = 0x00000005  (BLTU funcionou)
 * [5] = 0x00000006  (BGEU funcionou)
 * [6] = 0x000000FF  (Teste completo)
 */

#define DEBUG ((volatile unsigned int *)0x20000000)

int main(void) {
  int a = 5;
  int b = 5;
  int c = 10;
  int neg = -5;

  // BEQ (branch if equal)
  if (a == b) {
    DEBUG[0] = 1; // Deve executar
  } else {
    DEBUG[0] = 0xDEAD; // Erro!
  }

  // BNE (branch if not equal)
  if (a != c) {
    DEBUG[1] = 2; // Deve executar
  } else {
    DEBUG[1] = 0xDEAD;
  }

  // BLT (branch if less than - signed)
  if (neg < a) {
    DEBUG[2] = 3; // Deve executar (-5 < 5)
  } else {
    DEBUG[2] = 0xDEAD;
  }

  // BGE (branch if greater or equal - signed)
  if (a >= b) {
    DEBUG[3] = 4; // Deve executar (5 >= 5)
  } else {
    DEBUG[3] = 0xDEAD;
  }

  // BLTU (branch if less than - unsigned)
  unsigned int u1 = 5;
  unsigned int u2 = 10;
  if (u1 < u2) {
    DEBUG[4] = 5; // Deve executar
  } else {
    DEBUG[4] = 0xDEAD;
  }

  // BGEU (branch if greater or equal - unsigned)
  if (u2 >= u1) {
    DEBUG[5] = 6; // Deve executar
  } else {
    DEBUG[5] = 0xDEAD;
  }

  // Marcador de conclusão
  DEBUG[6] = 0xFF;

  return 0;
}
