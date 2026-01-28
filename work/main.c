#define DEBUG ((volatile unsigned int *)0x20000000)

int main(void) {
  volatile unsigned int a = 0;
  volatile unsigned int b = 1;
  volatile unsigned int next;
  volatile unsigned int i;

  for (i = 0; i < 10; ++i) {
    DEBUG[i] = a; /* write word at base + 4*i */
    next = a + b;
    a = b;
    b = next;
  }

  /* valor de retorno (vai ficar em a0) */
  return 0x2D;
}
