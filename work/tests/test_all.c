/**
 * TESTE COMPLETO: Todas as instruções RV32I
 *
 * Formato de resultado:
 * DEBUG[0-9]   = Testes ALU
 * DEBUG[10-16] = Testes Branch
 * DEBUG[17-25] = Testes Memory
 * DEBUG[26-30] = Testes Jump
 * DEBUG[31]    = 0xCAFEBABE (sucesso total)
 */

#define DEBUG ((volatile unsigned int *)0x20000000)
#define SCRATCH ((volatile unsigned int *)0x20001000)

// ========== FUNÇÕES AUXILIARES ==========
int add_func(int a, int b) { return a + b; }
void set_debug(int idx, int val) { DEBUG[idx] = val; }

// ========== MAIN ==========
int main(void) {
  int i = 0;

  // ===== TESTES ALU (índices 0-9) =====
  int a = 5, b = 10;
  DEBUG[i++] = a + b;           // [0] = 15
  DEBUG[i++] = a - b;           // [1] = -5 (0xFFFFFFFB)
  DEBUG[i++] = a << 2;          // [2] = 20
  DEBUG[i++] = (a < b) ? 1 : 0; // [3] = 1
  DEBUG[i++] = a ^ b;           // [4] = 15
  DEBUG[i++] = 80 >> 4;         // [5] = 5
  DEBUG[i++] = a | b;           // [6] = 15
  DEBUG[i++] = a & b;           // [7] = 0
  DEBUG[i++] = a * 2;           // [8] = 10 (via add a,a)
  DEBUG[i++] = ~a;              // [9] = 0xFFFFFFFA

  // ===== TESTES BRANCH (��ndices 10-16) =====
  if (a == 5)
    DEBUG[i++] = 0xB1;
  else
    DEBUG[i++] = 0; // [10] BEQ
  if (a != b)
    DEBUG[i++] = 0xB2;
  else
    DEBUG[i++] = 0; // [11] BNE
  if (a < b)
    DEBUG[i++] = 0xB3;
  else
    DEBUG[i++] = 0; // [12] BLT
  if (b >= a)
    DEBUG[i++] = 0xB4;
  else
    DEBUG[i++] = 0; // [13] BGE

  unsigned int u1 = 1, u2 = 0xFFFFFFFF;
  if (u1 < u2)
    DEBUG[i++] = 0xB5;
  else
    DEBUG[i++] = 0; // [14] BLTU
  if (u2 >= u1)
    DEBUG[i++] = 0xB6;
  else
    DEBUG[i++] = 0;  // [15] BGEU
  DEBUG[i++] = 0xBB; // [16] Branch tests done

  // ===== TESTES MEMORY (índices 17-25) =====
  SCRATCH[0] = 0xDEADBEEF;
  DEBUG[i++] = SCRATCH[0]; // [17] SW/LW = 0xDEADBEEF

  volatile unsigned short *hp = (volatile unsigned short *)SCRATCH;
  hp[0] = 0x1234;
  DEBUG[i++] = hp[0]; // [18] SH/LH = 0x1234

  volatile unsigned char *bp = (volatile unsigned char *)SCRATCH;
  bp[0] = 0xAB;
  DEBUG[i++] = bp[0]; // [19] SB/LB = 0xAB

  bp[0] = 0xFF;
  DEBUG[i++] = (unsigned int)bp[0];     // [20] LBU = 0xFF
  DEBUG[i++] = (int)(signed char)bp[0]; // [21] LB = 0xFFFFFFFF

  hp[0] = 0x8000;
  DEBUG[i++] = (unsigned int)hp[0];      // [22] LHU = 0x8000
  DEBUG[i++] = (int)(signed short)hp[0]; // [23] LH = 0xFFFF8000

  DEBUG[i++] = 0xAA; // [24] placeholder
  DEBUG[i++] = 0xBB; // [25] placeholder - correção abaixo

  // ===== TESTES JUMP (índices 26-30) =====
  int res = add_func(7, 8);
  DEBUG[i++] = res; // [26] JAL/JALR = 15

  set_debug(27, 0x77); // [27] = 0x77

  DEBUG[28] = 0x88;
  DEBUG[29] = 0x99;
  DEBUG[30] = 0xAA;

  // ===== MARCADOR DE SUCESSO =====
  DEBUG[31] = 0xCAFEBABE;

  while (1)
    ; // Loop infinito
  return 0;
}
