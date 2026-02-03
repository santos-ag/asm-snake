// Jogo da Cobrinha para RISC-V 32I - Varredura da tela

// Registradores de memória
#define REG_COR (*(volatile unsigned char *)512)
#define REG_X (*(volatile unsigned char *)516)
#define REG_Y (*(volatile unsigned char *)520)
#define REG_RESET (*(volatile unsigned char *)524)
#define REG_CLOCK (*(volatile unsigned char *)528)

#define COR_VERDE 0x1C
#define COR_PRETO 0x00
#define TAMANHO 6

// Função para desenhar pixel
void desenha(unsigned char x, unsigned char y, unsigned char cor) {
  REG_COR = cor;
  REG_X = x & 0x0F;
  REG_Y = y & 0x0F;
  REG_CLOCK = 0x00;
  REG_CLOCK = 0x01;
  REG_CLOCK = 0x00;
}

void main(void) {
  // Posições da cobra
  unsigned char px[TAMANHO];
  unsigned char py[TAMANHO];

  unsigned char cabeca_x = 5;
  unsigned char cabeca_y = 0;
  unsigned char rabo_x, rabo_y;
  unsigned char i;

  // ========== RESET DA TELA ==========
  REG_CLOCK = 0x00;
  REG_RESET = 0x01;
  REG_RESET = 0x00;

  // ========== INICIALIZA COBRA (horizontal na linha 0) ==========
  px[0] = 5;
  py[0] = 0;
  px[1] = 4;
  py[1] = 0;
  px[2] = 3;
  py[2] = 0;
  px[3] = 2;
  py[3] = 0;
  px[4] = 1;
  py[4] = 0;
  px[5] = 0;
  py[5] = 0;

  cabeca_x = 5;
  cabeca_y = 0;

  // Desenha cobra inicial
  for (i = 0; i < TAMANHO; i++) {
    desenha(px[i], py[i], COR_VERDE);
  }

  // ========== LOOP PRINCIPAL ==========
  while (1) {
    // Delay
    for (volatile unsigned int d = 0; d < 20000; d++)
      ;

    // Guarda posição do rabo ANTES de mover
    rabo_x = px[TAMANHO - 1];
    rabo_y = py[TAMANHO - 1];

    // Move corpo (cada segmento assume posição do anterior)
    px[5] = px[4];
    py[5] = py[4];
    px[4] = px[3];
    py[4] = py[3];
    px[3] = px[2];
    py[3] = py[2];
    px[2] = px[1];
    py[2] = py[1];
    px[1] = px[0];
    py[1] = py[0];

    // Move cabeça: X incrementa
    cabeca_y = cabeca_y + 1;
    /*if (cabeca_x >= 8) {
      cabeca_y = cabeca_y + 1;
    }*/
    // Quando X chega a 16 (passa de 15), volta para 0 e Y incrementa
    /*if (cabeca_x >= 15) {
      cabeca_x = 0;
      cabeca_y = cabeca_y + 1;

      // Quando Y chega a 16, volta para 0
      if (cabeca_y > 15) {
        cabeca_y = 0;
      }
    }*/

    // Atualiza posição da cabeça no array
    px[0] = cabeca_x;
    py[0] = cabeca_y;

    // Apaga rabo antigo (PRETO)
    desenha(rabo_x, rabo_y, COR_PRETO);

    // Desenha nova cabeça (VERDE)
    desenha(cabeca_x, cabeca_y, COR_VERDE);
  }
}
