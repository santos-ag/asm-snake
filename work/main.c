/* main.c - Programa de Teste para RV32I */

/* Endereços de memória para visualizar no Logisim */
/* IMPORTANTE: Estes endereços devem estar ACIMA do código! */
#define ADDR_A 0x00000100   /* Word 0x40 */
#define ADDR_B 0x00000104   /* Word 0x41 */
#define ADDR_RES 0x00000108 /* Word 0x42 */
#define ADDR_CNT 0x0000010C /* Word 0x43 */

/* Função principal - SEM retorno de valor */
void main(void) {
  /* Ponteiros volatile para forçar escrita na memória */
  volatile int *ptr_a = (volatile int *)ADDR_A;
  volatile int *ptr_b = (volatile int *)ADDR_B;
  volatile int *ptr_res = (volatile int *)ADDR_RES;
  volatile int *ptr_cnt = (volatile int *)ADDR_CNT;

  /* Variáveis locais (serão alocadas na stack) */
  int a = 10;
  int b = 20;
  int soma = 0;
  int contador = 0;
  int i;

  /* Loop para testar ALU e branches */
  for (i = 0; i < 5; i++) {
    soma = soma + a + b;
    contador++;
  }

  /* Escreve resultados na RAM */
  *ptr_a = a;          /* 0x0000000A (10) */
  *ptr_b = b;          /* 0x00000014 (20) */
  *ptr_res = soma;     /* 0x00000096 (150) */
  *ptr_cnt = contador; /* 0x00000005 (5) */

  /* Loop infinito - processador para aqui */
  while (1) {
    /* Nada - apenas espera */
  }
}
