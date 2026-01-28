#define DEBUG_ADDR ((volatile int *)0x20000000)

int main(void) {
  *DEBUG_ADDR = 45;
  while (1)
    ;
}
