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

void gameloop();
void define_map();
void init_entiys();
void move_pacman();
void mossa_utente();
void event_buff();


int game_running = 1;
int map[map_height][map_width];
int entitys[map_height][map_width];
int pacman_x = 1;
int pacman_y = 0;
int input_recived = 0;
int points = 0;


int main(){ 

    return 0;
}

void gameloop(){
    while(game_running){

    }
}


void define_map(){

    char mappa[map_height][map_width] = {
        "############################",
        "#************##************#",
        "#*####*#####*##*#####*####*#",
        "#*####*#####*##*#####*####*#",
        "#*####*#####*##*#####*####*#",
        "#**************************#",
        "#*####*##*########*##*####*#",
        "#o####*##*########*##*####o#",
        "#******##****##****##******#",
        "######*##### ## #####*######",
        "     #*##### ## #####*#     ",
        "     #*##          ##*#     ",
        "######*## ###__### ##*######",
        "      *   #      #   *      ",
        "######*## ######## ##*######",
        "     #*##          ##*#     ",
        "     #*## ######## ##*#     ",
        "######*## ######## ##*######",
        "#************##************#",
        "#*####*#####*##*#####*####*#",
        "#*####*#####*##*#####*####*#",
        "#o**##*              *##**o#",
        "###*##*##*########*##*##*###",
        "###*##*##*########*##*##*###",
        "#******##****##****##******#",
        "#*########## ## ##########*#",
        "#*########## ## ##########*#",
        "#**************************#",
        "############################"

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
    entitys[13][11] = GHOST;
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

        if (c == 'w' && pacman_y == 0 && input_recived == 0){
            pacman_y = -1;
            pacman_x = 0;
            input_recived = 1;
        }

        if (c == 'a' && pacman_x == 0 && input_recived == 0){
            pacman_x = -1;
            pacman_y = 0;
            input_recived = 1;
        }

        if (c == 's' && pacman_y == 0 && input_recived == 0){
            pacman_y = 1;
            pacman_x = 0;
            input_recived = 1;
        }
        
        if (c == 'd' && pacman_x == 0 && input_recived == 0){
            pacman_x = 1;
            pacman_y = 0;
            input_recived = 1;
        }
        
    

    endwin();
}

void move_pacman(){
    int pacman_pos_x;
    int pacman_pos_y;
    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            if(map[i][j] == PACMAN){
                pacman_pos_x = j;
                pacman_pos_y = i;
            }
        }
    }
    int old_pacman_pos_x = pacman_pos_x;
    int old_pacman_pos_y = pacman_pos_y;

    if(pacman_x == 1){
        pacman_pos_x++;
    }else if(pacman_pos_x == -1){
        pacman_pos_x--;
    }else if(pacman_pos_y == 1){
        pacman_pos_y--;
    }else if(pacman_pos_y == -1){
        pacman_pos_y++;
    }

    if(map[pacman_pos_y][pacman_pos_x] != WALL && map[pacman_pos_y][pacman_pos_x] != GATE){
        if(entitys[pacman_pos_y][pacman_pos_x] == GHOST){
            game_running = 0;
        }else if(entitys[pacman_pos_y][pacman_pos_x] == EGHOST){
            points += 400;
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
        }else if(map[pacman_pos_x][pacman_pos_y] == ORB){
            points += 10;
        }else if(map[pacman_pos_x][pacman_pos_y] == APPLE){
            points += 500;
        }else if(map[pacman_pos_x][pacman_pos_y] == BUFF){
            event_buff();
            points += 100;
        }else{
            entitys[pacman_pos_y][pacman_pos_x] = PACMAN;
        }

        entitys[old_pacman_pos_y][old_pacman_pos_x] = 0;

    }
}

void event_buff(){
    // evento da definire per quando il pacman mangia un buff
}