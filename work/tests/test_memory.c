/**
 * TESTE 3: Instruções de Load/Store
 *
 * Testa LW, SW, LH, SH, LB, SB, LHU, LBU
 *
 * Resultados esperados em DEBUG (0x20000000):
 * [0] = 0x12345678  (SW/LW funcionou)
 * [1] = 0x00005678  (SH/LH lower half)
 * [2] = 0x00001234  (SH/LH upper half)
 * [3] = 0x00000078  (SB/LB byte 0)
 * [4] = 0x00000056  (SB/LB byte 1)
 * [5] = 0x000000FF  (LBU - unsigned byte)
 * [6] = 0xFFFFFFFF  (LB - signed byte extension)
 * [7] = 0x0000FFFF  (LHU - unsigned halfword)
 * [8] = 0xFFFFFFFF  (LH - signed halfword extension)
 */

#define DEBUG ((volatile unsigned int *)0x20000000)
#define SCRATCH ((volatile unsigned char *)0x20001000)

int main(void) {
  volatile unsigned int *word_ptr = (volatile unsigned int *)SCRATCH;
  volatile unsigned short *half_ptr = (volatile unsigned short *)SCRATCH;
  volatile unsigned char *byte_ptr = (volatile unsigned char *)SCRATCH;

  // Teste SW/LW (Store Word / Load Word)
  *word_ptr = 0x12345678;
  DEBUG[0] = *word_ptr; // Esperado: 0x12345678

  // Teste SH/LH (Store Halfword / Load Halfword)
  half_ptr[0] = 0x5678;
  half_ptr[1] = 0x1234;
  DEBUG[1] = half_ptr[0]; // Esperado: 0x5678
  DEBUG[2] = half_ptr[1]; // Esperado: 0x1234

  // Teste SB/LB (Store Byte / Load Byte)
  byte_ptr[0] = 0x78;
  byte_ptr[1] = 0x56;
  DEBUG[3] = byte_ptr[0]; // Esperado: 0x78
  DEBUG[4] = byte_ptr[1]; // Esperado: 0x56

  // Teste LBU vs LB (unsigned vs signed extension)
  byte_ptr[0] = 0xFF;
  DEBUG[5] = (unsigned int)byte_ptr[0];     // LBU: Esperado 0x000000FF
  DEBUG[6] = (int)(signed char)byte_ptr[0]; // LB: Esperado 0xFFFFFFFF (-1)

  // Teste LHU vs LH (unsigned vs signed extension)
  half_ptr[0] = 0xFFFF;
  DEBUG[7] = (unsigned int)half_ptr[0];      // LHU: Esperado 0x0000FFFF
  DEBUG[8] = (int)(signed short)half_ptr[0]; // LH: Esperado 0xFFFFFFFF (-1)

  return 0;
}
