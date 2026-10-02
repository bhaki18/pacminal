#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FPS (1000000 / 2) // 2fps
#define map_width 28      // larghezza della mappa in caratteri ASCII
#define map_height 29     // altezza della mappa in caratteri ASCII
#define WALL 1000
#define GATE 1500
#define GHOST 2000
#define PACMAN 3000
#define ORB 4000
#define BUFF 5000
#define APPLE 6000
#define EGHOST 7000
#define VOID 8000
#define BUFF_LAST 20
#define SPAWN_RATE 60

void gameloop();
void define_map();
void init_entiys();
void move_pacman();
void mossa_utente();
void event_buff();
void buff_handler();
void print_map();
void pulisci_terminale();
void handle_ghosts();
void move_ghost(int ghost_x, int ghost_y, int pac_x, int pac_y, int type);
int try_move(int x, int y);
void show_win_screen();
void show_lose_screen();

int game_running = 1;
int map[map_height][map_width];
int entitys[map_height][map_width];
int pacman_x = 1;
int pacman_y = 0;

int points = 0;
int waiter = 0;
int buff_active = 0;
int time_s = 0;

int main() {
  define_map();
  init_entiys();
  gameloop();
  return 0;
}

void gameloop() {
  while (game_running) {
    mossa_utente();
    pulisci_terminale();
    mossa_utente();
    move_pacman();
    mossa_utente();
    handle_ghosts();
    buff_handler();
    mossa_utente();
    print_map();
    mossa_utente();
    usleep(FPS);
    time_s += 1;

    // Controllo vittoria: nessun ORB rimasto sulla mappa
    int orbs_left = 0;
    for (int i = 0; i < map_height; i++) {
      for (int j = 0; j < map_width; j++) {
        if (map[i][j] == ORB || map[i][j] == BUFF) {
          orbs_left++;
        }
      }
    }
    if (orbs_left == 0) {
      show_win_screen();
      return;
    }
  }
  // Sconfitta: il giocatore è stato mangiato
  show_lose_screen();
}

void define_map() {

  char mappa[map_height][map_width] = {
      "############################", // 0
      "#************##************#", // 1
      "#*####*#####*##*#####*####*#", // 2
      "#*####*#####*##*#####*####*#", // 3
      "#*####*#####*##*#####*####*#", // 4
      "#**************************#", // 5
      "#*####*##*########*##*####*#", // 6
      "#o####*##*########*##*####o#", // 7
      "#******##****##****##******#", // 8
      "######*##### ## #####*######", // 9
      "     #*##### ## #####*#     ", // 10
      "     #*##          ##*#     ", // 11
      "######*## ###__### ##*######", // 12
      "      *   #      #   *      ", // 13
      "######*## ######## ##*######", // 14
      "     #*##          ##*#     ", // 15
      "     #*## ######## ##*#     ", // 16
      "######*## ######## ##*######", // 17
      "#************##************#", // 18
      "#*####*#####*##*#####*####*#", // 19
      "#*####*#####*##*#####*####*#", // 20
      "#o**##*              *##**o#", // 21
      "###*##*##*########*##*##*###", // 22
      "###*##*##*########*##*##*###", // 23
      "#******##****##****##******#", // 24
      "#*########## ## ##########*#", // 25
      "#*########## ## ##########*#", // 26
      "#**************************#", // 27
      "############################"  // 28

  };

  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      if (mappa[i][j] == '#') {
        map[i][j] = WALL;
      } else if (mappa[i][j] == '_') {
        map[i][j] = GATE;
      } else if (mappa[i][j] == 'o') {
        map[i][j] = BUFF;
      } else if (mappa[i][j] == '*') {
        map[i][j] = ORB;
      } else {
        map[i][j] = VOID;
      }
    }
  }
}

void init_entiys() {
  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      entitys[i][j] = 0;
    }
  }

  entitys[21][13] = PACMAN;
  entitys[11][11] = GHOST;
  entitys[13][12] = GHOST;
  entitys[13][13] = GHOST;
  entitys[13][14] = GHOST;
}

void mossa_utente() {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);

  int c = getch();

  if (c == 'w') {
    pacman_y = 1;
    pacman_x = 0;
  }

  if (c == 'a') {
    pacman_x = -1;
    pacman_y = 0;
  }

  if (c == 's') {
    pacman_y = -1;
    pacman_x = 0;
  }

  if (c == 'd') {
    pacman_x = 1;
    pacman_y = 0;
  }

  endwin();
}

void move_pacman() {
  int pacman_pos_x = -1;
  int pacman_pos_y = -1;
  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      if (entitys[i][j] == PACMAN) {
        pacman_pos_x = j;
        pacman_pos_y = i;
      }
    }
  }

  if (pacman_pos_x == -1 && pacman_pos_y == -1) {
    printf("pacman non trovato");
    return;
  }

  int old_pacman_pos_x = pacman_pos_x;
  int old_pacman_pos_y = pacman_pos_y;

  if (pacman_x == 1 && pacman_pos_x < map_width - 1) {
    pacman_pos_x++;
  } else if (pacman_x == -1 && pacman_pos_x > 0) {
    pacman_pos_x--;
  } else if (pacman_y == 1 && pacman_pos_y > 0) {
    pacman_pos_y--;
  } else if (pacman_y == -1 && pacman_pos_y < map_height - 1) {
    pacman_pos_y++;
  }

  if (pacman_pos_y == 13 && pacman_pos_x == 0) {
    entitys[pacman_pos_y][pacman_pos_x] = 0;
    if (entitys[13][27] == GHOST) {
      game_running = 0;
    } else if (entitys[13][27] == EGHOST) {
      points += 400;
      entitys[13][27] = PACMAN;
    } else {
      entitys[13][27] = PACMAN;
    }

    return;
  } else if (pacman_pos_y == 13 && pacman_pos_x == 27) {
    entitys[old_pacman_pos_y][old_pacman_pos_x] = 0;
    if (entitys[13][0] == GHOST) {
      game_running = 0;
    } else if (entitys[13][0] == EGHOST) {
      points += 400;
      entitys[13][0] = PACMAN;
    } else {
      entitys[13][0] = PACMAN;
    }
    return;
  }
  if (map[pacman_pos_y][pacman_pos_x] != WALL &&
      map[pacman_pos_y][pacman_pos_x] != GATE) {
    if (entitys[pacman_pos_y][pacman_pos_x] == GHOST) {
      game_running = 0;
    } else if (entitys[pacman_pos_y][pacman_pos_x] == EGHOST) {
      points += 400;
      entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
    } else if (map[pacman_pos_y][pacman_pos_x] == ORB) {
      points += 10;
      entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
      map[pacman_pos_y][pacman_pos_x] = VOID;
    } else if (map[pacman_pos_y][pacman_pos_x] == APPLE) {
      points += 500;
      map[pacman_pos_y][pacman_pos_x] = VOID;
      entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
    } else if (map[pacman_pos_y][pacman_pos_x] == BUFF) {
      event_buff();
      points += 100;
      map[pacman_pos_y][pacman_pos_x] = VOID;
      entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
    } else {
      entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
    }
    entitys[old_pacman_pos_y][old_pacman_pos_x] = 0;
  }
}

void event_buff() {
  // evento da definire per quando il pacman mangia un buff
  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      if (entitys[i][j] == GHOST) {
        entitys[i][j] = EGHOST;
      }
    }
  }

  waiter = BUFF_LAST;
  buff_active = 1;
}

void buff_handler() {
  if (buff_active) {
    waiter--;
    if (!waiter) {
      buff_active = 0;
      for (int i = 0; i < map_height; i++) {
        for (int j = 0; j < map_width; j++) {
          if (entitys[i][j] == EGHOST) {
            entitys[i][j] = GHOST;
          }
        }
      }
    }
  }
}

void print_map() {

  int real_map[map_height][map_width];

  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      real_map[i][j] = map[i][j];
    }
  }

  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      if (entitys[i][j] != 0) {
        real_map[i][j] = entitys[i][j];
      }
    }
  }

  char printable_map[map_height][map_width];
  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      int cat = real_map[i][j];
      if (cat == WALL) {
        printable_map[i][j] = '#';
      } else if (cat == GATE) {
        printable_map[i][j] = '_';
      } else if (cat == GHOST) {
        printable_map[i][j] = 'B';
      } else if (cat == PACMAN) {
        printable_map[i][j] = 'C';
      } else if (cat == ORB) {
        printable_map[i][j] = '*';
      } else if (cat == BUFF) {
        printable_map[i][j] = 'o';
      } else if (cat == APPLE) {
        printable_map[i][j] = '@';
      } else if (cat == EGHOST) {
        printable_map[i][j] = 'A';
      } else if (cat == VOID) {
        printable_map[i][j] = ' ';
      }
    }
  }

  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      printf("%c", printable_map[i][j]);
    }
    printf("\n");
  }

  printf("points:%d\n", points);
}

void pulisci_terminale() { printf("\033[2J\033[H"); }

void handle_ghosts() {
  int pac_x = -1;
  int pac_y = -1;
  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      if (entitys[i][j] == PACMAN) {
        pac_x = j;
        pac_y = i;
      }
    }
  }

  // Raccogliamo i fantasmi prima di muoverli per evitare che uno stesso
  // fantasma venga spostato più volte nello stesso turno
  int ghosts_x[100];
  int ghosts_y[100];
  int ghosts_type[100];
  int ghost_count = 0;

  for (int i = 0; i < map_height; i++) {
    for (int j = 0; j < map_width; j++) {
      if (entitys[i][j] == GHOST || entitys[i][j] == EGHOST) {
        if (ghost_count < 100) {
          ghosts_x[ghost_count] = j;
          ghosts_y[ghost_count] = i;
          ghosts_type[ghost_count] = entitys[i][j];
          ghost_count++;
        }
      }
    }
  }

  for (int k = 0; k < ghost_count; k++) {
    // Verifichiamo che il fantasma sia ancora lì (p.es. non mangiato)
    if (entitys[ghosts_y[k]][ghosts_x[k]] == ghosts_type[k]) {
      move_ghost(ghosts_x[k], ghosts_y[k], pac_x, pac_y, ghosts_type[k]);
    }
  }

  if (SPAWN_RATE == (time_s / 2)) {
    if (entitys[13][11] == GHOST || entitys[13][11] == EGHOST) {
      entitys[13][11] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][12] == GHOST || entitys[13][12] == EGHOST) {
      entitys[13][12] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][13] == GHOST || entitys[13][13] == EGHOST) {
      entitys[13][13] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][14] == GHOST || entitys[13][14] == EGHOST) {
      entitys[13][14] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][15] == GHOST || entitys[13][15] == EGHOST) {
      entitys[13][15] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][16] == GHOST || entitys[13][16] == EGHOST) {
      entitys[13][16] = 0;
      entitys[11][11] = GHOST;
    }
  }

  if (SPAWN_RATE * 2 == (time_s / 2)) {
    if (entitys[13][11] == GHOST || entitys[13][11] == EGHOST) {
      entitys[13][11] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][12] == GHOST || entitys[13][12] == EGHOST) {
      entitys[13][12] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][13] == GHOST || entitys[13][13] == EGHOST) {
      entitys[13][13] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][14] == GHOST || entitys[13][14] == EGHOST) {
      entitys[13][14] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][15] == GHOST || entitys[13][15] == EGHOST) {
      entitys[13][15] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][16] == GHOST || entitys[13][16] == EGHOST) {
      entitys[13][16] = 0;
      entitys[11][11] = GHOST;
    }
  }

  if (SPAWN_RATE * 3 == (time_s / 2)) {
    if (entitys[13][11] == GHOST || entitys[13][11] == EGHOST) {
      entitys[13][11] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][12] == GHOST || entitys[13][12] == EGHOST) {
      entitys[13][12] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][13] == GHOST || entitys[13][13] == EGHOST) {
      entitys[13][13] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][14] == GHOST || entitys[13][14] == EGHOST) {
      entitys[13][14] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][15] == GHOST || entitys[13][15] == EGHOST) {
      entitys[13][15] = 0;
      entitys[11][11] = GHOST;
    } else if (entitys[13][16] == GHOST || entitys[13][16] == EGHOST) {
      entitys[13][16] = 0;
      entitys[11][11] = GHOST;
    }
  }
}

void move_ghost(int ghost_x, int ghost_y, int pac_x, int pac_y, int type) {
  // Se pacman non è presente sulla mappa, non muovere
  if (pac_x == -1 || pac_y == -1) {
    return;
  }

  // Definiamo le 4 direzioni possibili: Destra, Sinistra, Giù, Su
  int dirs[4][2] = {
      {1, 0},   // 0: Destra
      {-1, 0},  // 1: Sinistra
      {0, 1},   // 2: Giù
      {0, -1}   // 3: Su
  };

  int scores[4] = {0, 0, 0, 0};

  for (int i = 0; i < 4; i++) {
    int next_x = ghost_x + dirs[i][0];
    int next_y = ghost_y + dirs[i][1];

    if (!try_move(next_x, next_y)) {
      scores[i] = -999999;
      continue;
    }

    // Calcolo della distanza Manhattan dalla posizione futura a Pacman
    int dist = abs(next_x - pac_x) + abs(next_y - pac_y);

    if (type == GHOST) {
      // Inseguimento: distanza minore = score migliore (più alto)
      scores[i] = -dist;
    } else {
      // EGHOST (fuga): distanza maggiore = score migliore (più alto)
      scores[i] = dist;
    }
  }

  // Troviamo la direzione valida con il punteggio migliore
  int best_dir = -1;
  int best_score = -99999;

  for (int i = 0; i < 4; i++) {
    if (scores[i] > best_score) {
      best_score = scores[i];
      best_dir = i;
    }
  }

  // Se c'è una mossa valida, esegui lo spostamento
  if (best_dir != -1 && best_score > -999999) {
    int target_x = ghost_x + dirs[best_dir][0];
    int target_y = ghost_y + dirs[best_dir][1];

    // Se il fantasma tocca Pacman
    if (entitys[target_y][target_x] == PACMAN) {
      if (type == GHOST) {
        game_running = 0;
      } else if (type == EGHOST) {
        points += 400;
        entitys[ghost_y][ghost_x] = 0;
        return;
      }
    }

    entitys[target_y][target_x] = type;
    entitys[ghost_y][ghost_x] = 0;
  }
}

int try_move(int x, int y) {
  // Controllo limiti della mappa
  if (x < 0 || x >= map_width || y < 0 || y >= map_height) {
    return 0;
  }

  // Controllo collisioni con muri o cancelli
  if (map[y][x] == GATE || map[y][x] == WALL) {
    return 0;
  }

  // Controllo collisioni con altri fantasmi
  if (entitys[y][x] == GHOST || entitys[y][x] == EGHOST) {
    return 0;
  }

  return 1;
}

void show_win_screen() {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, FALSE); // Bloccante: aspetta input
  curs_set(0);

  clear();

  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  // Cornice
  for (int c = 0; c < cols; c++) {
    mvaddch(0, c, '=');
    mvaddch(rows - 1, c, '=');
  }
  for (int r = 0; r < rows; r++) {
    mvaddch(r, 0, '|');
    mvaddch(r, cols - 1, '|');
  }

  // Titolo e testo centrati
  int center_row = rows / 2;
  int center_col = cols / 2;

  const char *title   = "*** YOU WIN! ***";
  const char *sub     = "Hai mangiato tutte le pillole!";
  char score_msg[64];
  snprintf(score_msg, sizeof(score_msg), "Punteggio finale: %d", points);
  const char *prompt  = "Premi un tasto per uscire...";

  mvaddstr(center_row - 3, center_col - (int)(sizeof("*** YOU WIN! ***") / 2), title);
  mvaddstr(center_row - 1, center_col - (int)(sizeof("Hai mangiato tutte le pillole!") / 2), sub);
  mvaddstr(center_row + 1, center_col - (int)(strlen(score_msg) / 2), score_msg);
  mvaddstr(center_row + 3, center_col - (int)(sizeof("Premi un tasto per uscire...") / 2), prompt);

  refresh();
  getch(); // Aspetta input
  endwin();
}

void show_lose_screen() {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, FALSE); // Bloccante: aspetta input
  curs_set(0);

  clear();

  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  // Cornice
  for (int c = 0; c < cols; c++) {
    mvaddch(0, c, '=');
    mvaddch(rows - 1, c, '=');
  }
  for (int r = 0; r < rows; r++) {
    mvaddch(r, 0, '|');
    mvaddch(r, cols - 1, '|');
  }

  // Titolo e testo centrati
  int center_row = rows / 2;
  int center_col = cols / 2;

  const char *title   = "*** GAME OVER ***";
  const char *sub     = "Sei stato mangiato da un fantasma!";
  char score_msg[64];
  snprintf(score_msg, sizeof(score_msg), "Punteggio finale: %d", points);
  const char *prompt  = "Premi un tasto per uscire...";

  mvaddstr(center_row - 3, center_col - (int)(sizeof("*** GAME OVER ***") / 2), title);
  mvaddstr(center_row - 1, center_col - (int)(sizeof("Sei stato mangiato da un fantasma!") / 2), sub);
  mvaddstr(center_row + 1, center_col - (int)(strlen(score_msg) / 2), score_msg);
  mvaddstr(center_row + 3, center_col - (int)(sizeof("Premi un tasto per uscire...") / 2), prompt);

  refresh();
  getch(); // Aspetta input
  endwin();
}