// =============================================================================
// JOGO DA COBRINHA - VERSÃO VIDEO RGB
// Para Logisim Evolution com display RGB 16x16
// =============================================================================

// Mapeamento de memória
#define VIDEO_BASE 0x00000400 // Base para Video RGB (framebuffer)
#define INPUT_BASE 0x00000900 // Base para entrada (botões)

// Registradores de I/O
#define VIDEO_ADDR ((volatile unsigned int *)VIDEO_BASE)
#define INPUT_ADDR ((volatile unsigned int *)INPUT_BASE)

// Configurações do jogo
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 16
#define MAX_SNAKE_LENGTH (SCREEN_WIDTH * SCREEN_HEIGHT)

// =============================================================================
// CORES RGB (formato 0x00RRGGBB)
// =============================================================================
#define COLOR_BLACK 0x00000000   // Fundo
#define COLOR_GREEN 0x0000FF00   // Corpo da cobra
#define COLOR_LIME 0x0000FF80    // Cabeça da cobra (verde claro)
#define COLOR_RED 0x00FF0000     // Comida
#define COLOR_DARKRED 0x00880000 // Comida alternativa (para piscar)
#define COLOR_WHITE 0x00FFFFFF   // Texto/bordas
#define COLOR_GRAY 0x00444444    // Game over fundo
#define COLOR_YELLOW 0x00FFFF00  // Efeito especial

// Direções
typedef enum {
  DIR_UP = 0,
  DIR_RIGHT = 1,
  DIR_DOWN = 2,
  DIR_LEFT = 3
} Direction;

// Posição
typedef struct {
  unsigned char x;
  unsigned char y;
} Position;

// Estado do jogo
typedef struct {
  Position snake[MAX_SNAKE_LENGTH];
  unsigned int length;
  Direction direction;
  Position food;
  unsigned int score;
  unsigned int game_over;
  unsigned int frame_count; // Para animações (piscar comida)
} GameState;

// Estado do jogo (variável global)
GameState game;

// Seed para gerador de números aleatórios
static unsigned int rand_seed = 12345;

// =============================================================================
// FUNÇÕES AUXILIARES
// =============================================================================

// Módulo unsigned (RV32I não tem instrução DIV/MOD)
unsigned int umod(unsigned int a, unsigned int b) {
  if (b == 0)
    return 0;
  if (a < b)
    return a;

  while (a >= b) {
    a = a - b;
  }
  return a;
}

// Gerador de números pseudo-aleatórios (LCG)
unsigned int random_number(void) {
  rand_seed = (rand_seed * 1103515245 + 12345) & 0x7FFFFFFF;
  return rand_seed;
}

// Delay simples
void delay(unsigned int cycles) {
  for (volatile unsigned int i = 0; i < cycles; i++) {
    asm volatile("nop");
  }
}

// =============================================================================
// FUNÇÕES DE DESENHO PARA VIDEO RGB
// =============================================================================

// Calcula o índice do pixel no framebuffer
// Fórmula: índice = y * SCREEN_WIDTH + x
static inline unsigned int pixel_index(unsigned int x, unsigned int y) {
  return y * SCREEN_WIDTH + x;
}

// Desenha um pixel na posição (x, y) com a cor especificada
void draw_pixel(unsigned int x, unsigned int y, unsigned int color) {
  if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
    VIDEO_ADDR[pixel_index(x, y)] = color;
  }
}

// Limpa toda a tela com uma cor
void clear_screen(unsigned int color) {
  for (unsigned int y = 0; y < SCREEN_HEIGHT; y++) {
    for (unsigned int x = 0; x < SCREEN_WIDTH; x++) {
      VIDEO_ADDR[pixel_index(x, y)] = color;
    }
  }
}

// Desenha um retângulo preenchido
void draw_rect(unsigned int x1, unsigned int y1, unsigned int x2,
               unsigned int y2, unsigned int color) {
  for (unsigned int y = y1; y <= y2 && y < SCREEN_HEIGHT; y++) {
    for (unsigned int x = x1; x <= x2 && x < SCREEN_WIDTH; x++) {
      draw_pixel(x, y, color);
    }
  }
}

// =============================================================================
// FUNÇÕES DO JOGO
// =============================================================================

// Inicializa o jogo
void game_init(void) {
  // Limpa array da cobra
  for (int i = 0; i < MAX_SNAKE_LENGTH; i++) {
    game.snake[i].x = 0;
    game.snake[i].y = 0;
  }

  // Posição inicial da cobra (centro da tela)
  game.snake[0].x = SCREEN_WIDTH / 2;
  game.snake[0].y = SCREEN_HEIGHT / 2;
  game.length = 3;

  // Corpo inicial (horizontal, para a esquerda)
  for (unsigned int i = 1; i < game.length; i++) {
    game.snake[i].x = game.snake[0].x - i;
    game.snake[i].y = game.snake[0].y;
  }

  // Direção inicial
  game.direction = DIR_RIGHT;

  // Comida inicial em posição aleatória
  game.food.x = umod(random_number(), (SCREEN_WIDTH - 2)) + 1;
  game.food.y = umod(random_number(), (SCREEN_HEIGHT - 2)) + 1;

  game.score = 0;
  game.game_over = 0;
  game.frame_count = 0;

  // Limpa a tela
  clear_screen(COLOR_BLACK);
}

// Verifica colisão com a própria cobra
int check_self_collision(unsigned char x, unsigned char y) {
  for (unsigned int i = 1; i < game.length; i++) {
    if (game.snake[i].x == x && game.snake[i].y == y) {
      return 1;
    }
  }
  return 0;
}

// Verifica colisão com parede
int check_wall_collision(unsigned char x, unsigned char y) {
  return (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT);
}

// Gera nova posição para comida
void spawn_food(void) {
  int valid = 0;
  while (!valid) {
    game.food.x = umod(random_number(), (SCREEN_WIDTH - 2)) + 1;
    game.food.y = umod(random_number(), (SCREEN_HEIGHT - 2)) + 1;

    // Verifica se não está na cobra
    valid = 1;
    for (unsigned int i = 0; i < game.length; i++) {
      if (game.snake[i].x == game.food.x && game.snake[i].y == game.food.y) {
        valid = 0;
        break;
      }
    }
  }
}

// Lê entrada dos botões
// Bits: [3]=LEFT, [2]=DOWN, [1]=RIGHT, [0]=UP
unsigned int get_input(void) { return *INPUT_ADDR & 0x0F; }

// Atualiza lógica do jogo
void game_update(void) {
  if (game.game_over)
    return;

  // Incrementa contador de frames (para animações)
  game.frame_count++;

  // Lê entrada
  unsigned int input = get_input();

  // Atualiza direção (não pode reverter 180°)
  if ((input & (1 << DIR_UP)) && game.direction != DIR_DOWN) {
    game.direction = DIR_UP;
  } else if ((input & (1 << DIR_RIGHT)) && game.direction != DIR_LEFT) {
    game.direction = DIR_RIGHT;
  } else if ((input & (1 << DIR_DOWN)) && game.direction != DIR_UP) {
    game.direction = DIR_DOWN;
  } else if ((input & (1 << DIR_LEFT)) && game.direction != DIR_RIGHT) {
    game.direction = DIR_LEFT;
  }

  // Salva posição da cauda ANTES de mover (para apagar)
  unsigned char tail_x = game.snake[game.length - 1].x;
  unsigned char tail_y = game.snake[game.length - 1].y;

  // Calcula nova posição da cabeça
  unsigned char new_x = game.snake[0].x;
  unsigned char new_y = game.snake[0].y;

  switch (game.direction) {
  case DIR_UP:
    new_y--;
    break;
  case DIR_DOWN:
    new_y++;
    break;
  case DIR_LEFT:
    new_x--;
    break;
  case DIR_RIGHT:
    new_x++;
    break;
  }

  // Verifica colisões
  if (check_wall_collision(new_x, new_y) ||
      check_self_collision(new_x, new_y)) {
    game.game_over = 1;
    return;
  }

  // Verifica se comeu ANTES de mover o corpo
  int ate_food = (new_x == game.food.x && new_y == game.food.y);

  // Move o corpo (cada segmento assume posição do anterior)
  for (int i = game.length - 1; i > 0; i--) {
    game.snake[i] = game.snake[i - 1];
  }

  // Atualiza cabeça
  game.snake[0].x = new_x;
  game.snake[0].y = new_y;

  // Se comeu, cresce e gera nova comida
  if (ate_food) {
    if (game.length < MAX_SNAKE_LENGTH) {
      game.length++;
      game.score += 10;
    }
    spawn_food();
  } else {
    // Se não comeu, apaga a posição antiga da cauda
    draw_pixel(tail_x, tail_y, COLOR_BLACK);
  }
}

// Renderiza o jogo no Video RGB
void game_render(void) {
  // Desenha a cabeça da cobra (cor diferente)
  draw_pixel(game.snake[0].x, game.snake[0].y, COLOR_LIME);

  // Desenha o corpo da cobra
  for (unsigned int i = 1; i < game.length; i++) {
    draw_pixel(game.snake[i].x, game.snake[i].y, COLOR_GREEN);
  }

  // Desenha comida (pisca alternando cores)
  unsigned int food_color;
  if (umod(game.frame_count, 8) < 4) {
    food_color = COLOR_RED;
  } else {
    food_color = COLOR_DARKRED;
  }
  draw_pixel(game.food.x, game.food.y, food_color);

  // Se game over, mostra efeito visual
  if (game.game_over) {
    // Pisca a cobra em vermelho
    if (umod(game.frame_count, 4) < 2) {
      for (unsigned int i = 0; i < game.length; i++) {
        draw_pixel(game.snake[i].x, game.snake[i].y, COLOR_RED);
      }
    } else {
      for (unsigned int i = 0; i < game.length; i++) {
        draw_pixel(game.snake[i].x, game.snake[i].y, COLOR_GRAY);
      }
    }
  }
}

// =============================================================================
// FUNÇÃO PRINCIPAL
// =============================================================================

int main(void) {
  // Inicializa jogo
  game_init();

  // Loop principal
  while (1) {
    game_update();
    game_render();
    delay(50000); // Ajuste para velocidade desejada

    // Reinicia se game over e botão pressionado
    if (game.game_over) {
      delay(200000); // Pausa antes de permitir restart
      if (get_input()) {
        game_init();
      }
    }
  }

  return 0;
}
