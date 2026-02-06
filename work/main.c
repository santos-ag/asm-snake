// Jogo da Cobrinha - Uma tecla por vez (edge detection)

#define REG_COR (*(volatile unsigned char *)0x200)
#define REG_X (*(volatile unsigned char *)0x204)
#define REG_Y (*(volatile unsigned char *)0x208)
#define REG_RESET (*(volatile unsigned char *)0x20C)
#define REG_CLOCK (*(volatile unsigned char *)0x210)
#define REG_INPUT (*(volatile unsigned char *)0x214)

#define COR_VERDE 0x0000FF00
#define COR_PRETO 0x00000000
#define COR_VERMELHO 0x00FF0000
#define TAMANHO 4

#define DIR_CIMA    0
#define DIR_BAIXO   1
#define DIR_ESQ     2
#define DIR_DIR     3

#define TECLA_W     87
#define TECLA_w     119
#define TECLA_S     83
#define TECLA_s     115
#define TECLA_A     65
#define TECLA_a     97
#define TECLA_D     68
#define TECLA_d     100

// --- Mover os vetores para escopo global (evita colisão com MMIO na stack) ---
static unsigned char px[TAMANHO];
static unsigned char py[TAMANHO];

void desenha(unsigned char x, unsigned char y, int cor) {
  REG_COR = cor;
  REG_X = x & 0x0F;
  REG_Y = y & 0x0F;
  REG_CLOCK = 0x00;
  REG_CLOCK = 0x01;
  REG_CLOCK = 0x00;
}

void main(void) {
  unsigned char i;
  unsigned char direcao = DIR_DIR;
  unsigned char tecla_atual;
  unsigned char tecla_anterior = 0;  // guarda última tecla
  unsigned char rabo_x, rabo_y;
  unsigned char fruit_x, fruit_y;
  // Reset da tela (apenas na inicialização)
  REG_CLOCK = 0x00;
  REG_RESET = 0x01;
  REG_RESET = 0x00;
  fruit_x = 3;
  fruit_y = 3;
  // Inicializa cobra
  px[0] = 7;  py[0] = 7;
  px[1] = 6;  py[1] = 7;
  px[2] = 5;  py[2] = 7;
  px[3] = 4;  py[3] = 7;

  for (i = 0; i < TAMANHO; i++) {
    desenha(px[i], py[i], COR_VERDE);
  }
  desenha(fruit_x, fruit_y, 0xFF);
  // Loop principal
  while (1) {
    for (volatile unsigned int d = 0; d < 500000000; d++);

    // Lê tecla
    tecla_atual = REG_INPUT & 0x7F;

    // Só processa se mudou
    if (tecla_atual != tecla_anterior && tecla_atual != 0) {
      if ((tecla_atual == TECLA_W || tecla_atual == TECLA_w) && direcao != DIR_BAIXO) {
        direcao = DIR_CIMA;
      }
      else if ((tecla_atual == TECLA_S || tecla_atual == TECLA_s) && direcao != DIR_CIMA) {
        direcao = DIR_BAIXO;
      }
      else if ((tecla_atual == TECLA_A || tecla_atual == TECLA_a) && direcao != DIR_DIR) {
        direcao = DIR_ESQ;
      }
      else if ((tecla_atual == TECLA_D || tecla_atual == TECLA_d) && direcao != DIR_ESQ) {
        direcao = DIR_DIR;
      }
      tecla_anterior = tecla_atual;
    }
    if (tecla_atual == 0) tecla_anterior = 0;

    // Grava rabo antes de mover
    rabo_x = px[TAMANHO - 1];
    rabo_y = py[TAMANHO - 1];

    // Move
    for (i = TAMANHO - 1; i > 0; i--) {
      px[i] = px[i - 1];
      py[i] = py[i - 1];
    }

    if (direcao == DIR_CIMA) {
      py[0] = (py[0] == 0) ? 15 : py[0] - 1;
    }
    else if (direcao == DIR_BAIXO) {
      py[0] = (py[0] == 15) ? 0 : py[0] + 1;
    }
    else if (direcao == DIR_ESQ) {
      px[0] = (px[0] == 0) ? 15 : px[0] - 1;
    }
    else if (direcao == DIR_DIR) {
      px[0] = (px[0] == 15) ? 0 : px[0] + 1;
    }

    // Apaga rabo antigo e desenha nova cabeça
    desenha(rabo_x, rabo_y, COR_PRETO);
    desenha(px[0], py[0], COR_VERDE);
  }
}
