#include <stdio.h>
#include <ncurses.h>

#define FPS (1000000/2) // 2fps  
#define map_width 28 // larghezza della mappa in caratteri ASCII
#define map_height 29   // altezza della mappa in caratteri ASCII
#define WALL 1000
#define GHOST 2000
#define PACMAN 3000
#define ORB 4000
#define BUFF 5000
#define APPLE 6000
#define EGHOST 7000

void gameloop();
void define_map();

int game_running = 1;
int map[map_height][map_width];

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
        "#●####*##*########*##*####●#",
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
        "#●**##*              *##**●#",
        "###*##*##*########*##*##*###",
        "###*##*##*########*##*##*###",
        "#******##****##****##******#",
        "#*########## ## ##########*#",
        "#*########## ## ##########*#",
        "#**************************#",
        "############################"

    }

    for(int i = 0;i<map_height;i++){
        for(int j = 0;j<map_width;j++){
            if(mappa[i][j] == "#"){
                map[i][j] = WALL;
            }else if(mappa[i][j] == "●"){
                map[i][j] = BUFF;
            }else if(mappa[i][j] == "*"){
                map[i][j] = ORB;
            }
        }
    }

}