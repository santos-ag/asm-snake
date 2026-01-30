/* main.c - Programa de Teste para RV32I */

// Endereços de memória para visualizar no Logisim
#define ADDR_A 0x00000100
#define ADDR_B 0x00000104
#define ADDR_RES 0x00000108
#define ADDR_CNT 0x0000010C

void main() {
  volatile int *ptr_a = (int *)ADDR_A;
  volatile int *ptr_b = (int *)ADDR_B;
  volatile int *ptr_res = (int *)ADDR_RES;
  volatile int *ptr_cnt = (int *)ADDR_CNT;

  int a = 10;
  int b = 20;
  int soma = 0;
  int contador = 0;

  // Loop para testar ALU e saltos
  for (int i = 0; i < 5; i++) {
    soma = soma + a + b;
    contador++;
  }

  // Escreve resultados na RAM (visível no Logisim)
  *ptr_a = a;          // 0x0000000A (10)
  *ptr_b = b;          // 0x00000014 (20)
  *ptr_res = soma;     // 0x00000096 (150)
  *ptr_cnt = contador; // 0x00000005 (5)

  // Loop infinito
  while (1)
    ;
}
