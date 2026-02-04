// Jogo da Cobrinha para RISC-V 32I - Varredura da tela

// Registradores de memória
#define REG_COR (*(volatile unsigned char *)512)      // 0x200
#define REG_X (*(volatile unsigned char *)516)        // 0x204
#define REG_Y (*(volatile unsigned char *)520)        // 0x208
#define REG_RESET (*(volatile unsigned char *)524)    // 0x20C
#define REG_CLOCK (*(volatile unsigned char *)528)    // 0x210
#define REG_INPUT (*(volatile unsigned char *)532)    // 0x214 - NOVO!

#define COR_VERDE 0x1C
#define COR_VERMELHO 0xE0
#define COR_PRETO 0x00
#define TAMANHO 6

// Códigos de tecla
#define TECLA_CIMA    0x01
#define TECLA_BAIXO   0x02
#define TECLA_ESQ     0x03
#define TECLA_DIR     0x04

// Direções
#define DIR_CIMA    0
#define DIR_BAIXO   1
#define DIR_ESQ     2
#define DIR_DIR     3

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
  unsigned char cabeca_y = 5;
  unsigned char rabo_x, rabo_y;
  unsigned char i;
  unsigned char direcao = DIR_DIR; // Começa indo para direita
  unsigned char tecla;

  // ========== RESET DA TELA ==========
  REG_CLOCK = 0x00;
  REG_RESET = 0x01;
  REG_RESET = 0x00;

  // ========== INICIALIZA COBRA (horizontal) ==========
  px[0] = 5; py[0] = 5;
  px[1] = 4; py[1] = 5;
  px[2] = 3; py[2] = 5;
  px[3] = 2; py[3] = 5;
  px[4] = 1; py[4] = 5;
  px[5] = 0; py[5] = 5;

  // Desenha cobra inicial
  for (i = 0; i < TAMANHO; i++) {
    desenha(px[i], py[i], COR_VERDE);
  }

  // ========== LOOP PRINCIPAL ==========
  while (1) {
    // Delay
    for (volatile unsigned int d = 0; d < 20000; d++);

    // ========== LÊ INPUT DO TECLADO ==========
    tecla = REG_INPUT;
    
    // Atualiza direção (impede voltar na direção oposta)
    if (tecla == TECLA_CIMA && direcao != DIR_BAIXO) {
      direcao = DIR_CIMA;
    } else if (tecla == TECLA_BAIXO && direcao != DIR_CIMA) {
      direcao = DIR_BAIXO;
    } else if (tecla == TECLA_ESQ && direcao != DIR_DIR) {
      direcao = DIR_ESQ;
    } else if (tecla == TECLA_DIR && direcao != DIR_ESQ) {
      direcao = DIR_DIR;
    }

    // Guarda posição do rabo ANTES de mover
    rabo_x = px[TAMANHO - 1];
    rabo_y = py[TAMANHO - 1];

    // Move corpo (cada segmento assume posição do anterior)
    for (i = TAMANHO - 1; i > 0; i--) {
      px[i] = px[i - 1];
      py[i] = py[i - 1];
    }

    // Move cabeça conforme direção
    if (direcao == DIR_CIMA) {
      cabeca_y = (cabeca_y == 0) ? 15 : cabeca_y - 1;
    } else if (direcao == DIR_BAIXO) {
      cabeca_y = (cabeca_y == 15) ? 0 : cabeca_y + 1;
    } else if (direcao == DIR_ESQ) {
      cabeca_x = (cabeca_x == 0) ? 15 : cabeca_x - 1;
    } else if (direcao == DIR_DIR) {
      cabeca_x = (cabeca_x == 15) ? 0 : cabeca_x + 1;
    }

    // Atualiza posição da cabeça no array
    px[0] = cabeca_x;
    py[0] = cabeca_y;

    // Apaga rabo antigo (PRETO)
    desenha(rabo_x, rabo_y, COR_PRETO);

    // Desenha nova cabeça (VERDE)
    desenha(cabeca_x, cabeca_y, COR_VERDE);
  }
}
