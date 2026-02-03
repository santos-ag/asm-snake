// Mapeamento de memória
#define DISPLAY_BASE 0x00000400 // Base para display de LEDs
#define INPUT_BASE 0x00000500   // Base para entrada (botões)
#define TIMER_BASE 0x00000600   // Timer para controle de velocidade

// Registradores de I/O
#define DISPLAY_ADDR ((volatile unsigned int *)DISPLAY_BASE)
#define INPUT_ADDR ((volatile unsigned int *)INPUT_BASE)
#define TIMER_ADDR ((volatile unsigned int *)TIMER_BASE)

// Configurações do jogo
#define GRID_WIDTH 16
#define GRID_HEIGHT 16
#define MAX_SNAKE_LENGTH (GRID_WIDTH * GRID_HEIGHT)

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
} GameState;

// Estado do jogo
GameState game;

// Seed para gerador de números aleatórios
static unsigned int rand_seed = 12345;

/* ============================================
 *   FUNÇÃO AUXILIAR: MÓDULO UNSIGNED
 *   (RV32I não tem instrução DIV/MOD)
 *   ============================================ */
unsigned int umod(unsigned int a, unsigned int b) {
  if (b == 0)
    return 0; // Evita divisão por zero
  if (a < b)
    return a;

  // Subtração repetida
  while (a >= b) {
    a = a - b;
  }
  return a;
}

// Gerador de números pseudo-aleatórios simples (LCG)
unsigned int random_number(void) {
  rand_seed = (rand_seed * 1103515245 + 12345) & 0x7FFFFFFF;
  return rand_seed;
}

// Delay simples (ajuste conforme clock do processador)
void delay(unsigned int cycles) {
  for (volatile unsigned int i = 0; i < cycles; i++) {
    asm volatile("nop");
  }
}

// Inicializa o jogo
void game_init(void) {
  // Limpa tudo
  for (int i = 0; i < MAX_SNAKE_LENGTH; i++) {
    game.snake[i].x = 0;
    game.snake[i].y = 0;
  }

  // Posição inicial da cobra (centro)
  game.snake[0].x = GRID_WIDTH / 2;
  game.snake[0].y = GRID_HEIGHT / 2;
  game.length = 3;

  // Corpo inicial
  for (unsigned int i = 1; i < game.length; i++) {
    game.snake[i].x = game.snake[0].x - i;
    game.snake[i].y = game.snake[0].y;
  }

  // Direção inicial
  game.direction = DIR_RIGHT;

  // Comida inicial - USANDO umod() em vez de %
  game.food.x = umod(random_number(), (GRID_WIDTH - 2)) + 1;
  game.food.y = umod(random_number(), (GRID_HEIGHT - 2)) + 1;

  game.score = 0;
  game.game_over = 0;
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
  return (x >= GRID_WIDTH || y >= GRID_HEIGHT);
}

// Atualiza posição da comida
void spawn_food(void) {
  int valid = 0;
  while (!valid) {
    // USANDO umod() em vez de %
    game.food.x = umod(random_number(), (GRID_WIDTH - 2)) + 1;
    game.food.y = umod(random_number(), (GRID_HEIGHT - 2)) + 1;

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

// Lê entrada (4 botões: UP, RIGHT, DOWN, LEFT)
unsigned int get_input(void) { return *INPUT_ADDR & 0x0F; }

// Atualiza lógica do jogo
void game_update(void) {
  if (game.game_over)
    return;

  // Lê entrada
  unsigned int input = get_input();

  // Atualiza direção (não pode reverter)
  if (input & (1 << DIR_UP) && game.direction != DIR_DOWN) {
    game.direction = DIR_UP;
  } else if (input & (1 << DIR_RIGHT) && game.direction != DIR_LEFT) {
    game.direction = DIR_RIGHT;
  } else if (input & (1 << DIR_DOWN) && game.direction != DIR_UP) {
    game.direction = DIR_DOWN;
  } else if (input & (1 << DIR_LEFT) && game.direction != DIR_RIGHT) {
    game.direction = DIR_LEFT;
  }

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

  // Move o corpo
  for (int i = game.length - 1; i > 0; i--) {
    game.snake[i] = game.snake[i - 1];
  }

  // Atualiza cabeça
  game.snake[0].x = new_x;
  game.snake[0].y = new_y;

  // Verifica se comeu
  if (new_x == game.food.x && new_y == game.food.y) {
    if (game.length < MAX_SNAKE_LENGTH) {
      game.length++;
      game.score += 10;
    }
    spawn_food();
  }
}

// Renderiza o jogo na memória de display
void game_render(void) {
  // Limpa display (256 words para 16x16 grid, 1 bit por pixel)
  // Usando 16 words de 16 bits cada (1 word por linha)
  for (int i = 0; i < GRID_HEIGHT; i++) {
    DISPLAY_ADDR[i] = 0;
  }

  // Desenha cobra
  for (unsigned int i = 0; i < game.length; i++) {
    unsigned char x = game.snake[i].x;
    unsigned char y = game.snake[i].y;

    if (x < GRID_WIDTH && y < GRID_HEIGHT) {
      DISPLAY_ADDR[y] |= (1 << x);
    }
  }

  // Desenha comida (piscando - bit mais alto indica blink)
  if (game.food.x < GRID_WIDTH && game.food.y < GRID_HEIGHT) {
    DISPLAY_ADDR[game.food.y] |= (1 << game.food.x);
  }

  // Escreve score em endereço separado
  DISPLAY_ADDR[GRID_HEIGHT] = game.score;

  // Game over flag
  DISPLAY_ADDR[GRID_HEIGHT + 1] = game.game_over;
}

// Função main
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
