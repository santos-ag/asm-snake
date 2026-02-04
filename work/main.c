// Jogo da Cobrinha - Versão de Teste (só movimentação)

// Registradores de memória
#define REG_COR (*(volatile unsigned char *)0x200)
#define REG_X (*(volatile unsigned char *)0x204)
#define REG_Y (*(volatile unsigned char *)0x208)
#define REG_RESET (*(volatile unsigned char *)0x20C)
#define REG_CLOCK (*(volatile unsigned char *)0x210)
#define REG_INPUT (*(volatile unsigned char *)0x214)  // LEITURA dos botões

// Cores
#define COR_VERDE 0x1C
#define COR_PRETO 0x00

// Tamanho fixo da cobra
#define TAMANHO 4

// Direções
#define DIR_CIMA    0
#define DIR_BAIXO   1
#define DIR_ESQ     2
#define DIR_DIR     3

// Botões (bits do REG_INPUT)
#define BTN_CIMA    0x01  // bit 0
#define BTN_BAIXO   0x02  // bit 1
#define BTN_ESQ     0x04  // bit 2
#define BTN_DIR     0x08  // bit 3

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
  // Arrays da cobra
  unsigned char px[TAMANHO];
  unsigned char py[TAMANHO];
  
  unsigned char i;
  unsigned char direcao = DIR_DIR;  // Começa indo para direita
  unsigned char tecla;
  unsigned char rabo_x, rabo_y;

  // ========== RESET DA TELA ==========
  REG_CLOCK = 0x00;
  REG_RESET = 0x01;
  REG_RESET = 0x00;

  // ========== INICIALIZA COBRA (horizontal no centro) ==========
  px[0] = 7;  py[0] = 7;  // Cabeça
  px[1] = 6;  py[1] = 7;
  px[2] = 5;  py[2] = 7;
  px[3] = 4;  py[3] = 7;  // Rabo

  // Desenha cobra inicial
  for (i = 0; i < TAMANHO; i++) {
    desenha(px[i], py[i], COR_VERDE);
  }

  // ========== LOOP PRINCIPAL ==========
  while (1) {
    
    // Delay (ajuste conforme necessário)
    for (volatile unsigned int d = 0; d < 50000; d++);

    // ========== LÊ OS BOTÕES ==========
    tecla = REG_INPUT;

    // Atualiza direção (impede voltar para direção oposta)
    if ((tecla & BTN_CIMA) && direcao != DIR_BAIXO) {
      direcao = DIR_CIMA;
    }
    else if ((tecla & BTN_BAIXO) && direcao != DIR_CIMA) {
      direcao = DIR_BAIXO;
    }
    else if ((tecla & BTN_ESQ) && direcao != DIR_DIR) {
      direcao = DIR_ESQ;
    }
    else if ((tecla & BTN_DIR) && direcao != DIR_ESQ) {
      direcao = DIR_DIR;
    }

    // Salva posição do rabo (para apagar)
    rabo_x = px[TAMANHO - 1];
    rabo_y = py[TAMANHO - 1];

    // Move o corpo (cada segmento vai para posição do anterior)
    for (i = TAMANHO - 1; i > 0; i--) {
      px[i] = px[i - 1];
      py[i] = py[i - 1];
    }

    // Move a cabeça na direção atual
    if (direcao == DIR_CIMA) {
      py[0] = (py[0] == 0) ? 15 : py[0] - 1;  // Wrap around
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

    // Apaga o rabo antigo
    desenha(rabo_x, rabo_y, COR_PRETO);

    // Desenha a nova cabeça
    desenha(px[0], py[0], COR_VERDE);
  }
}
