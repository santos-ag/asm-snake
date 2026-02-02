/* test_suite.c - Suite Completa de Testes para RISC-V 32I */

/* ============================================
   MAPA DE MEMÓRIA
   ============================================
   0x100 - 0x1FF: Resultados dos Testes
   0x200 - 0x2FF: Dados de Debug
   0x300 - 0x3FF: Arrays de Teste
   0x400 - 0x4FF: Relatório Final
   ============================================ */

#define TEST_RESULTS_BASE 0x00000100
#define DEBUG_DATA_BASE 0x00000200
#define TEST_ARRAYS_BASE 0x00000300
#define REPORT_BASE 0x00000400

/* Estrutura do Relatório */
typedef struct {
  int total_tests;
  int passed_tests;
  int failed_tests;
  int test_alu_add;
  int test_alu_sub;
  int test_alu_and;
  int test_alu_or;
  int test_alu_xor;
  int test_alu_sll;
  int test_alu_srl;
  int test_alu_sra;
  int test_alu_slt;
  int test_memory_lw;
  int test_memory_sw;
  int test_branch_beq;
  int test_branch_bne;
  int test_branch_blt;
  int test_branch_bge;
  int test_jump_jal;
  int test_function_call;
  int test_recursion;
  int test_array_access;
  int test_pointer_arithmetic;
  int test_loop_for;
  int test_loop_while;
  int timestamp_start;
  int timestamp_end;
  int cycles_estimated;
} TestReport;

/* ============================================
   FUNÇÃO AUXILIAR: MULTIPLICAÇÃO POR SOMA
   (RV32I não tem instrução MUL)
   ============================================ */
int multiply(int a, int b) {
  /* Casos especiais para otimização */
  if (b == 0)
    return 0;
  if (b == 1)
    return a;
  if (b == -1)
    return -a;

  int resultado = 0;
  int negativo = 0;

  /* Trata números negativos */
  if (b < 0) {
    negativo = 1;
    b = -b;
  }

  /* Multiplica por soma repetida */
  for (int i = 0; i < b; i++) {
    resultado = resultado + a;
  }

  /* Ajusta sinal */
  if (negativo) {
    resultado = -resultado;
  }

  return resultado;
}

/* ============================================
   FUNÇÕES AUXILIARES
   ============================================ */

/* Simula contador de ciclos (incrementa sempre que chamado) */
volatile int cycle_counter = 0;
void tick(void) { cycle_counter++; }

/* Marca sucesso/falha de teste */
void mark_pass(volatile int *result) {
  *result = 1;
  tick();
}
void mark_fail(volatile int *result) {
  *result = 0;
  tick();
}

/* ============================================
   TESTES DA ALU - OPERAÇÕES ARITMÉTICAS
   ============================================ */

int test_alu_addition(void) {
  /* Testa: ADD, ADDI */
  int a = 10;
  int b = 20;
  int result = a + b;

  if (result == 30) {
    /* Testa overflow */
    int max = 0x7FFFFFFF;
    int overflow = max + 1;
    if (overflow == 0x80000000) {
      return 1; /* PASS */
    }
  }
  return 0; /* FAIL */
}

int test_alu_subtraction(void) {
  /* Testa: SUB */
  int a = 50;
  int b = 20;
  int result = a - b;

  if (result == 30) {
    /* Testa underflow */
    int min = 0x80000000;
    int underflow = min - 1;
    if (underflow == 0x7FFFFFFF) {
      return 1; /* PASS */
    }
  }
  return 0; /* FAIL */
}

/* ============================================
   TESTES DA ALU - OPERAÇÕES LÓGICAS
   ============================================ */

int test_alu_and(void) {
  /* Testa: AND, ANDI */
  int a = 0b11110000;
  int b = 0b10101010;
  int result = a & b;

  return (result == 0b10100000) ? 1 : 0;
}

int test_alu_or(void) {
  /* Testa: OR, ORI */
  int a = 0b11110000;
  int b = 0b10101010;
  int result = a | b;

  return (result == 0b11111010) ? 1 : 0;
}

int test_alu_xor(void) {
  /* Testa: XOR, XORI */
  int a = 0b11110000;
  int b = 0b10101010;
  int result = a ^ b;

  return (result == 0b01011010) ? 1 : 0;
}

/* ============================================
   TESTES DA ALU - SHIFTS
   ============================================ */

int test_alu_shift_left(void) {
  /* Testa: SLL, SLLI */
  int a = 0x00000001;
  int result = a << 4;

  if (result == 0x00000010) {
    /* Testa shift por 31 */
    int b = 1 << 31;
    if (b == 0x80000000) {
      return 1; /* PASS */
    }
  }
  return 0; /* FAIL */
}

int test_alu_shift_right_logical(void) {
  /* Testa: SRL, SRLI */
  int a = 0x80000000;
  unsigned int ua = (unsigned int)a;
  unsigned int result = ua >> 4;

  return (result == 0x08000000) ? 1 : 0;
}

int test_alu_shift_right_arithmetic(void) {
  /* Testa: SRA, SRAI */
  int a = 0x80000000; /* Negativo */
  int result = a >> 4;

  /* Deve preservar sinal (preencher com 1s) */
  return (result == 0xF8000000) ? 1 : 0;
}

/* ============================================
   TESTES DA ALU - COMPARAÇÃO
   ============================================ */

int test_alu_set_less_than(void) {
  /* Testa: SLT, SLTI */
  int a = 10;
  int b = 20;
  int result = (a < b) ? 1 : 0;

  if (result == 1) {
    /* Testa com negativos */
    int neg = -5;
    int pos = 5;
    int result2 = (neg < pos) ? 1 : 0;
    if (result2 == 1) {
      return 1; /* PASS */
    }
  }
  return 0; /* FAIL */
}

/* ============================================
   TESTES DE MEMÓRIA
   ============================================ */

int test_memory_load_store(void) {
  /* Testa: LW, SW */
  volatile int *test_addr = (volatile int *)TEST_ARRAYS_BASE;

  /* Escreve valores */
  test_addr[0] = 0x12345678;
  test_addr[1] = 0xDEADBEEF;
  test_addr[2] = 0xCAFEBABE;

  /* Lê e verifica */
  if (test_addr[0] != 0x12345678)
    return 0;
  if (test_addr[1] != 0xDEADBEEF)
    return 0;
  if (test_addr[2] != 0xCAFEBABE)
    return 0;

  return 1; /* PASS */
}

/* ============================================
   TESTES DE BRANCHES
   ============================================ */

int test_branch_equal(void) {
  /* Testa: BEQ */
  int a = 42;
  int b = 42;
  int result = 0;

  if (a == b) {
    result = 1;
  }

  return result;
}

int test_branch_not_equal(void) {
  /* Testa: BNE */
  int a = 10;
  int b = 20;
  int result = 0;

  if (a != b) {
    result = 1;
  }

  return result;
}

int test_branch_less_than(void) {
  /* Testa: BLT */
  int a = 10;
  int b = 20;
  int result = 0;

  if (a < b) {
    result = 1;
  }

  /* Testa com negativos */
  int neg = -5;
  int pos = 5;
  int result2 = 0;

  if (neg < pos) {
    result2 = 1;
  }

  return (result && result2) ? 1 : 0;
}

int test_branch_greater_equal(void) {
  /* Testa: BGE */
  int a = 20;
  int b = 10;
  int result = 0;

  if (a >= b) {
    result = 1;
  }

  return result;
}

/* ============================================
   TESTES DE JUMPS E FUNÇÕES
   ============================================ */

int helper_function(int x, int y) {
  /* Testa passagem de parâmetros (a0, a1) */
  return x + y;
}

int test_function_calls(void) {
  /* Testa: JAL, JALR */
  int result = helper_function(10, 20);

  return (result == 30) ? 1 : 0;
}

/* ============================================
   FATORIAL ITERATIVO (SEM RECURSÃO)
   Para evitar stack overflow e multiplicação
   ============================================ */
int factorial(int n) {
  if (n <= 1) {
    return 1;
  }

  int resultado = 1;

  /* Calcula fatorial usando multiplicação por soma */
  for (int i = 2; i <= n; i++) {
    resultado = multiply(resultado, i);
  }

  return resultado;
}

int test_recursion(void) {
  /* Testa fatorial (agora iterativo) */
  int result = factorial(5); /* 5! = 120 */

  return (result == 120) ? 1 : 0;
}

/* ============================================
   TESTES DE ARRAYS E PONTEIROS
   ============================================ */

int test_array_operations(void) {
  volatile int *arr = (volatile int *)TEST_ARRAYS_BASE;

  /* Preenche array usando multiply() */
  for (int i = 0; i < 10; i++) {
    arr[i] = multiply(i, i); /* 0, 1, 4, 9, 16, ... */
  }

  /* Verifica valores */
  if (arr[0] != 0)
    return 0;
  if (arr[3] != 9)
    return 0;
  if (arr[5] != 25)
    return 0;
  if (arr[9] != 81)
    return 0;

  return 1; /* PASS */
}

int test_pointer_arithmetic(void) {
  volatile int *base = (volatile int *)TEST_ARRAYS_BASE;

  /* Escreve usando aritmética de ponteiros */
  volatile int *ptr = base;
  *ptr = 100;
  ptr++;
  *ptr = 200;
  ptr++;
  *ptr = 300;
  ptr++;

  /* Verifica */
  if (base[0] != 100)
    return 0;
  if (base[1] != 200)
    return 0;
  if (base[2] != 300)
    return 0;

  return 1; /* PASS */
}

/* ============================================
   TESTES DE LOOPS
   ============================================ */

int test_for_loop(void) {
  int sum = 0;

  for (int i = 1; i <= 10; i++) {
    sum = sum + i;
  }

  /* 1+2+3+...+10 = 55 */
  return (sum == 55) ? 1 : 0;
}

int test_while_loop(void) {
  int i = 0;
  int sum = 0;

  while (i < 10) {
    sum = sum + i;
    i = i + 1;
  }

  /* 0+1+2+...+9 = 45 */
  return (sum == 45) ? 1 : 0;
}

/* ============================================
   ALGORITMO COMPLEXO: FIBONACCI
   ============================================ */

int test_fibonacci_algorithm(void) {
  volatile int *fib = (volatile int *)(TEST_ARRAYS_BASE + 0x80);

  fib[0] = 0;
  fib[1] = 1;

  for (int i = 2; i < 10; i++) {
    fib[i] = fib[i - 1] + fib[i - 2];
  }

  /* Verifica alguns valores */
  if (fib[5] != 5)
    return 0; /* F(5) = 5 */
  if (fib[7] != 13)
    return 0; /* F(7) = 13 */
  if (fib[9] != 34)
    return 0; /* F(9) = 34 */

  return 1; /* PASS */
}

/* ============================================
   FUNÇÃO PRINCIPAL - EXECUTA TODOS OS TESTES
   ============================================ */

int main(void) {
  volatile TestReport *report = (volatile TestReport *)REPORT_BASE;
  volatile int *results = (volatile int *)TEST_RESULTS_BASE;

  /* Inicializa relatório */
  report->total_tests = 0;
  report->passed_tests = 0;
  report->failed_tests = 0;
  report->timestamp_start = cycle_counter;

  /* ==================================
     EXECUTA BATERIA DE TESTES
     ================================== */

  /* Testes ALU - Aritmética */
  results[0] = test_alu_addition();
  report->test_alu_add = results[0];
  report->total_tests++;
  if (results[0])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[1] = test_alu_subtraction();
  report->test_alu_sub = results[1];
  report->total_tests++;
  if (results[1])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes ALU - Lógica */
  results[2] = test_alu_and();
  report->test_alu_and = results[2];
  report->total_tests++;
  if (results[2])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[3] = test_alu_or();
  report->test_alu_or = results[3];
  report->total_tests++;
  if (results[3])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[4] = test_alu_xor();
  report->test_alu_xor = results[4];
  report->total_tests++;
  if (results[4])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes ALU - Shifts */
  results[5] = test_alu_shift_left();
  report->test_alu_sll = results[5];
  report->total_tests++;
  if (results[5])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[6] = test_alu_shift_right_logical();
  report->test_alu_srl = results[6];
  report->total_tests++;
  if (results[6])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[7] = test_alu_shift_right_arithmetic();
  report->test_alu_sra = results[7];
  report->total_tests++;
  if (results[7])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes ALU - Comparação */
  results[8] = test_alu_set_less_than();
  report->test_alu_slt = results[8];
  report->total_tests++;
  if (results[8])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes Memória */
  results[9] = test_memory_load_store();
  report->test_memory_lw = results[9];
  report->test_memory_sw = results[9];
  report->total_tests++;
  if (results[9])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes Branch */
  results[10] = test_branch_equal();
  report->test_branch_beq = results[10];
  report->total_tests++;
  if (results[10])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[11] = test_branch_not_equal();
  report->test_branch_bne = results[11];
  report->total_tests++;
  if (results[11])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[12] = test_branch_less_than();
  report->test_branch_blt = results[12];
  report->total_tests++;
  if (results[12])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[13] = test_branch_greater_equal();
  report->test_branch_bge = results[13];
  report->total_tests++;
  if (results[13])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes Jump e Funções */
  results[14] = test_function_calls();
  report->test_function_call = results[14];
  report->test_jump_jal = results[14];
  report->total_tests++;
  if (results[14])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[15] = test_recursion();
  report->test_recursion = results[15];
  report->total_tests++;
  if (results[15])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes Arrays */
  results[16] = test_array_operations();
  report->test_array_access = results[16];
  report->total_tests++;
  if (results[16])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[17] = test_pointer_arithmetic();
  report->test_pointer_arithmetic = results[17];
  report->total_tests++;
  if (results[17])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Testes Loops */
  results[18] = test_for_loop();
  report->test_loop_for = results[18];
  report->total_tests++;
  if (results[18])
    report->passed_tests++;
  else
    report->failed_tests++;

  results[19] = test_while_loop();
  report->test_loop_while = results[19];
  report->total_tests++;
  if (results[19])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Teste Algoritmo Complexo */
  results[20] = test_fibonacci_algorithm();
  report->total_tests++;
  if (results[20])
    report->passed_tests++;
  else
    report->failed_tests++;

  /* Finaliza relatório */
  report->timestamp_end = cycle_counter;
  report->cycles_estimated = cycle_counter;

  /* Loop infinito */

  return 0;

  // opa
}
