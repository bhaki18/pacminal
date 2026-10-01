#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>


#define FPS (1000000/2) // 2fps  
#define map_width 28 // larghezza della mappa in caratteri ASCII
#define map_height 29   // altezza della mappa in caratteri ASCII
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
void move_ghost(int ghost_x,int ghost_y,int pac_x,int pac_y,int type);


int game_running = 1;
int map[map_height][map_width];
int entitys[map_height][map_width];
int pacman_x = 1;
int pacman_y = 0;

int points = 0;
int waiter = 0;
int buff_active = 0;
int time_s = 0;





int main(){ 
    define_map();
    init_entiys();
    gameloop();
    return 0;
}

void gameloop(){
    while(game_running){
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
    }
}


void define_map(){

    char mappa[map_height][map_width] = {
        "############################",// 0
        "#************##************#",// 1
        "#*####*#####*##*#####*####*#",// 2
        "#*####*#####*##*#####*####*#",// 3
        "#*####*#####*##*#####*####*#",// 4
        "#**************************#",// 5
        "#*####*##*########*##*####*#",// 6
        "#o####*##*########*##*####o#",// 7
        "#******##****##****##******#",// 8
        "######*##### ## #####*######",// 9
        "     #*##### ## #####*#     ",// 10
        "     #*##          ##*#     ",// 11
        "######*## ###__### ##*######",// 12
        "      *   #      #   *      ",// 13
        "######*## ######## ##*######",// 14
        "     #*##          ##*#     ",// 15
        "     #*## ######## ##*#     ",// 16
        "######*## ######## ##*######",// 17
        "#************##************#",// 18
        "#*####*#####*##*#####*####*#",// 19
        "#*####*#####*##*#####*####*#",// 20
        "#o**##*              *##**o#",// 21
        "###*##*##*########*##*##*###",// 22
        "###*##*##*########*##*##*###",// 23
        "#******##****##****##******#",// 24
        "#*########## ## ##########*#",// 25
        "#*########## ## ##########*#",// 26
        "#**************************#",// 27
        "############################"// 28

    };

    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            if(mappa[i][j] == '#'){
                map[i][j] = WALL;
            }else if(mappa[i][j] == '_'){
                map[i][j] = GATE;
            }else if(mappa[i][j] == 'o'){
                map[i][j] = BUFF;
            }else if(mappa[i][j] == '*'){
                map[i][j] = ORB;
            }else{
                map[i][j] = VOID;
            }
        }
    }

}

void init_entiys(){
    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            entitys[i][j] = 0;
        }
    }

    entitys[21][13] = PACMAN;
    entitys[11][11] = GHOST;
    entitys[13][12] = GHOST;
    entitys[13][13] = GHOST;
    entitys[13][14] = GHOST;
}

void mossa_utente(){
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);


        int c = getch();

        if (c == 'w'){
            pacman_y = 1;
            pacman_x = 0;
            
        }

        if (c == 'a'){
            pacman_x = -1;
            pacman_y = 0;
            
        }

        if (c == 's'){
            pacman_y = -1;
            pacman_x = 0;
            
        }
        
        if (c == 'd'){
            pacman_x = 1;
            pacman_y = 0;
            
        }
        
    

    endwin();
}

void move_pacman(){
    int pacman_pos_x = -1;
    int pacman_pos_y = -1;
    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            if(entitys[i][j] == PACMAN){
                pacman_pos_x = j;
                pacman_pos_y = i;
            }
        }
    }

    if(pacman_pos_x == -1 && pacman_pos_y == -1){
        printf("pacman non trovato");
        return;
    }

    int old_pacman_pos_x = pacman_pos_x;
    int old_pacman_pos_y = pacman_pos_y;

    if(pacman_x == 1 && pacman_pos_x<map_width-1){
        pacman_pos_x++;
    }else if(pacman_x == -1 && pacman_pos_x>0){
        pacman_pos_x--;
    }else if(pacman_y == 1 && pacman_pos_y > 0){
        pacman_pos_y--;
    }else if(pacman_y == -1 && pacman_pos_y<map_height-1){
        pacman_pos_y++;
    }

    if(pacman_pos_y == 13 && pacman_pos_x == 0){
        entitys[pacman_pos_y][pacman_pos_x] = 0;
        if(entitys[13][27] == GHOST){
            game_running = 0;
        }else if(entitys[13][27] == EGHOST){
            points+= 400;
            entitys[13][27] = PACMAN;
        }else{
            entitys[13][27] = PACMAN;
        }

        return;
    }else if(pacman_pos_y == 13 && pacman_pos_x == 27){
        entitys[old_pacman_pos_y][old_pacman_pos_x] = 0;
        if(entitys[13][0] == GHOST){
            game_running = 0;
        }else if(entitys[13][0] == EGHOST){
            points+= 400;
            entitys[13][0] = PACMAN;
        }else{
            entitys[13][0] = PACMAN;
        }
        return;
    }
    if(map[pacman_pos_y][pacman_pos_x] != WALL && map[pacman_pos_y][pacman_pos_x] != GATE){
        if(entitys[pacman_pos_y][pacman_pos_x] == GHOST){
            game_running = 0;
        }else if(entitys[pacman_pos_y][pacman_pos_x] == EGHOST){
            points += 400;
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
        }else if(map[pacman_pos_y][pacman_pos_x] == ORB){
            points += 10;
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
            map[pacman_pos_y][pacman_pos_x] = VOID;
        }else if(map[pacman_pos_y][pacman_pos_x] == APPLE){
            points += 500;
            map[pacman_pos_y][pacman_pos_x] = VOID;
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
        }else if(map[pacman_pos_y][pacman_pos_x] == BUFF){
            event_buff();
            points += 100;
            map[pacman_pos_y][pacman_pos_x] = VOID;
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
        }else{
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
        }
        entitys[old_pacman_pos_y][old_pacman_pos_x] = 0;

    }

    
}

void event_buff(){
    // evento da definire per quando il pacman mangia un buff
    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            if(entitys[i][j] == GHOST){
                entitys[i][j] = EGHOST;
            }
        }
    }

    waiter = BUFF_LAST;
    buff_active = 1;

}

void buff_handler(){
    if(buff_active){
            waiter--;
            if(!waiter){
                buff_active = 0;
                for(int i = 0;i<map_height;i++){
                    for(int j = 0;j<map_width;j++){
                        if(entitys[i][j] == EGHOST){
                            entitys[i][j] = GHOST;
                        }
                    }
                }
            }
        }
}


void print_map(){
    
    int real_map[map_height][map_width];

    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            real_map[i][j] = map[i][j];
        }
    }

    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            if(entitys[i][j] != 0){
                real_map[i][j] = entitys[i][j];
            }
        }
    }

    char printable_map[map_height][map_width];
    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            int cat = real_map[i][j];
            if(cat == WALL){
                printable_map[i][j] = '#';
            }else if(cat == GATE){
                printable_map[i][j] = '_';
            }else if(cat == GHOST){
                printable_map[i][j] = 'B';
            }else if(cat == PACMAN){
                printable_map[i][j] = 'C';
            }else if(cat == ORB){
                printable_map[i][j] = '*';
            }else if(cat == BUFF){
                printable_map[i][j] = 'o';
            }else if(cat == APPLE){
                printable_map[i][j] = '@';
            }else if(cat == EGHOST){
                printable_map[i][j] = 'A';
            }else if(cat == VOID){
                printable_map[i][j] = ' ';
            }
        }
    }

    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            printf("%c",printable_map[i][j]);
        }
        printf("\n");
    }

    printf("points:%d\n",points);
}

void pulisci_terminale() {
    printf("\033[2J\033[H");
}

void handle_ghosts(){
    int pac_x = -1;
    int pac_y = -1;
    for(int i = 0;i<map_height;i++){
        for(int j =0;j<map_width;j++){
            if(entitys[i][j] == PACMAN){
                pac_x = j;
                pac_y = i;
            }
        }
    }
    for(int i = 0;i<map_height;i++){
        for(int j =0;j<map_width;j++){
            if(entitys[i][j] == GHOST){
                move_ghost(j,i,pac_x,pac_y,GHOST);
            }else if(entitys[i][j] == EGHOST){
                move_ghost(j,i,pac_x,pac_y,EGHOST);
            }
        }
    }


    if(SPAWN_RATE == (time_s/2)){
        if(entitys[13][11] == GHOST || entitys[13][11] == EGHOST){
            entitys[13][11] = 0;
            entitys[11][11] = GHOST;
        } else if(entitys[13][12] == GHOST || entitys[13][12] == EGHOST){
            entitys[13][12] = 0;
            entitys[11][11] = GHOST; 
        } else if(entitys[13][13] == GHOST || entitys[13][13] == EGHOST){
            entitys[13][13] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][14] == GHOST || entitys[13][14] == EGHOST){
            entitys[13][14] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][15] == GHOST || entitys[13][15] == EGHOST){
            entitys[13][15] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][16] == GHOST || entitys[13][16] == EGHOST){
            entitys[13][16] = 0;
            entitys[11][11] = GHOST;
        }
    }

    if(SPAWN_RATE * 2 == (time_s/2)){
        if(entitys[13][11] == GHOST || entitys[13][11] == EGHOST){
            entitys[13][11] = 0;
            entitys[11][11] = GHOST;
        } else if(entitys[13][12] == GHOST || entitys[13][12] == EGHOST){
            entitys[13][12] = 0;
            entitys[11][11] = GHOST; 
        } else if(entitys[13][13] == GHOST || entitys[13][13] == EGHOST){
            entitys[13][13] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][14] == GHOST || entitys[13][14] == EGHOST){
            entitys[13][14] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][15] == GHOST || entitys[13][15] == EGHOST){
            entitys[13][15] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][16] == GHOST || entitys[13][16] == EGHOST){
            entitys[13][16] = 0;
            entitys[11][11] = GHOST;
        }
    }

    if(SPAWN_RATE * 3 == (time_s/2)){
        if(entitys[13][11] == GHOST || entitys[13][11] == EGHOST){
            entitys[13][11] = 0;
            entitys[11][11] = GHOST;
        } else if(entitys[13][12] == GHOST || entitys[13][12] == EGHOST){
            entitys[13][12] = 0;
            entitys[11][11] = GHOST; 
        } else if(entitys[13][13] == GHOST || entitys[13][13] == EGHOST){
            entitys[13][13] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][14] == GHOST || entitys[13][14] == EGHOST){
            entitys[13][14] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][15] == GHOST || entitys[13][15] == EGHOST){
            entitys[13][15] = 0;
            entitys[11][11] = GHOST;
        }else if(entitys[13][16] == GHOST || entitys[13][16] == EGHOST){
            entitys[13][16] = 0;
            entitys[11][11] = GHOST;
        }
    }

}

void move_ghost(int ghost_x,int ghost_y,int pac_x,int pac_y,int type){

    int move_left = 0;
    int move_right = 0;
    int move_up = 0;
    int move_down = 0;

    int delta_x = ghost_x - pac_x;
    int delta_y = ghost_y - pac_y;

    if(delta_x < 0){
        delta_x = -delta_x;
        move_left ++;
        move_right --;
    }else if(delta_x > 0){
        move_left --;
        move_right ++;
    }

    if(delta_y < 0){
        delta_y = -delta_y;
        move_down ++;
        move_up --;
    }else if(delta_y > 0){
        move_down --;
        move_up ++;
    }

    if(delta_x > delta_y){
        move_left++;
        move_right++;
    }

    if(delta_x < delta_y){
        move_up++;
        move_down++;
    }

    if(move_right > move_left && move_right > move_down && move_right > move_up){
        
    }



    
}