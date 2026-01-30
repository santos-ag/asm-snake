/**
 * TESTE 5: LUI e AUIPC
 *
 * Resultados esperados em DEBUG (0x20000000):
 * [0] = 0x12345000  (LUI carrega upper 20 bits)
 * [1] = 0xDEADB000  (LUI com valor grande)
 * [2] = 0x12345678  (LUI + ADDI para valor completo)
 * [3] = != 0        (AUIPC - endereço relativo ao PC)
 * [4] = 0x000000FF  (teste completo)
 */

#define DEBUG ((volatile unsigned int *)0x20000000)

int main(void) {
  // LUI carrega os 20 bits superiores
  unsigned int lui_val;
  asm volatile("lui %0, 0x12345" : "=r"(lui_val));
  DEBUG[0] = lui_val; // Esperado: 0x12345000

  // LUI com outro valor
  asm volatile("lui %0, 0xDEADB" : "=r"(lui_val));
  DEBUG[1] = lui_val; // Esperado: 0xDEADB000

  // LUI + ADDI para valor de 32 bits completo
  unsigned int full_val;
  asm volatile("lui %0, 0x12345\n"
               "addi %0, %0, 0x678"
               : "=r"(full_val));
  DEBUG[2] = full_val; // Esperado: 0x12345678

  // AUIPC (adiciona imediato ao PC)
  unsigned int auipc_val;
  asm volatile("auipc %0, 0" : "=r"(auipc_val));
  DEBUG[3] = auipc_val; // Deve ser um endereço válido (não zero)

  // Marcador de conclusão
  DEBUG[4] = 0xFF;

  return 0;
}
