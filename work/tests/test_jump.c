/**
 * TESTE 4: Instruções JAL e JALR
 *
 * Resultados esperados em DEBUG (0x20000000):
 * [0] = 0x00000001  (função chamada via JAL)
 * [1] = 0x00000002  (retorno via JALR funcionou)
 * [2] = 0x00000003  (função com parâmetro)
 * [3] = 0x0000000A  (retorno de valor: 10)
 * [4] = 0x000000FF  (teste completo)
 */

#define DEBUG ((volatile unsigned int *)0x20000000)

// Função simples para testar JAL
void funcao_simples(void) { DEBUG[0] = 1; }

// Função que retorna valor
int soma(int a, int b) { return a + b; }

// Função com parâmetro
void funcao_param(int valor) { DEBUG[2] = valor; }

int main(void) {
  // Teste JAL (chamada de função)
  funcao_simples();
  DEBUG[1] = 2; // Se chegou aqui, JALR (ret) funcionou

  // Teste função com parâmetro
  funcao_param(3);

  // Teste função com retorno
  int resultado = soma(4, 6);
  DEBUG[3] = resultado; // Esperado: 10

  // Marcador de conclusão
  DEBUG[4] = 0xFF;

  return 0;
}
