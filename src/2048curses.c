
#include <curses.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
// from curses.h
#define COLOR_BLACK	    0
#define COLOR_RED	    1
#define COLOR_GREEN	    2
#define COLOR_YELLOW    3
#define COLOR_BLUE	    4
#define COLOR_MAGENTA	5
#define COLOR_CYAN	    6
#define COLOR_WHITE	    7
// custom colors
#define COLOR_ORANGE 8
#define COLOR_MAROON 9
#define COLOR_LIGHTBLUE 10
#define COLOR_PINK 11
#define COLOR_GOLD 12

const char* gameboard[] = {
" ------------------------------- ",
"|       |       |       |       |",
"|       |       |       |       |",
"|       |       |       |       |",
" ------------------------------- ",
"|       |       |       |       |",
"|       |       |       |       |",
"|       |       |       |       |",
" ------------------------------- ",
"|       |       |       |       |",
"|       |       |       |       |",
"|       |       |       |       |",
" ------------------------------- ",
"|       |       |       |       |",
"|       |       |       |       |",
"|       |       |       |       |",
" ------------------------------- ",
};

int grid[4][4] = {{},{},{},{}};

int nrows = 17; 
int ncols = 33;

void draw_board(WINDOW* win);
void draw_values(WINDOW* win);

void tilt_up();
void tilt_down();
void tilt_left();
void tilt_right();

void add_random();

void init_array();



int main(void){
    initscr();
    WINDOW* gamewin = newwin(nrows, ncols, 1, 1);

    curs_set(0);
    cbreak(); 
    noecho(); 
    start_color();
    keypad(gamewin, TRUE);


    init_pair(1, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_BLACK, COLOR_LIGHTBLUE);
    init_pair(4, COLOR_BLACK, COLOR_GREEN);
    init_pair(5, COLOR_BLACK, COLOR_YELLOW);
    init_pair(6, COLOR_BLACK, COLOR_CYAN);
    init_pair(7, COLOR_BLACK, COLOR_PINK);
    init_pair(8, COLOR_BLACK, COLOR_MAROON);
    init_pair(9, COLOR_BLACK, COLOR_RED);
    init_pair(10, COLOR_BLACK, COLOR_ORANGE);
    init_pair(11, COLOR_RED, COLOR_GOLD);

    init_color(COLOR_BLACK, 0, 0, 0);
    init_color(COLOR_MAGENTA, 300, 0, 800);
    init_color(COLOR_LIGHTBLUE, 200, 300, 900);
    init_color(COLOR_BLUE, 100, 100, 600);
    init_color(COLOR_GREEN, 0, 800, 200);
    init_color(COLOR_PINK, 1000, 400, 400);
    init_color(COLOR_MAROON, 1000, 200, 100);
    init_color(COLOR_ORANGE, 1000, 500, 100);
    init_color(COLOR_GOLD, 1000, 900, 100);



    wattron(gamewin, COLOR_PAIR(0));

    int ch;
    short quit = false;

    init_array();

    draw_board(gamewin);
    draw_values(gamewin);
    wrefresh(gamewin);

    while(quit == false){

        // if user hasn't provided input
        if((ch = wgetch(gamewin)) == ERR){
            
        }else{
            switch(ch){
                case KEY_UP:
                    tilt_up();
                    break;
                case KEY_DOWN:
                    tilt_down();
                    break;
                case KEY_LEFT:
                    tilt_left();
                    break;
                case KEY_RIGHT:
                    tilt_right();
                    break;
                case 'q':
                    quit = true;
                    break;
            }
            add_random();
            wclear(gamewin);
        }

        wrefresh(gamewin);
        draw_board(gamewin);
        draw_values(gamewin);
    }

    delwin(gamewin);
    endwin();

    return EXIT_SUCCESS;
}

void init_array(){
    srand(time(0));

    int numsset = 0; 
    srand(time(0));

    while(numsset < 2){
        int x = rand()%4;
        int y = rand()%4;
        if(grid[x][y] == 0){
            grid[x][y] = 2;
            numsset++;
        }
    }
}

void draw_values(WINDOW* win){
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){

            if(grid[i][j] != 0){

                int color_pair = log(grid[i][j]) / log(2);
                
                wattron(win, COLOR_PAIR(color_pair));

                int txoffset = 1+8*i;
				int tyoffset = 1+4*j;

				for(size_t q = 0; q < 3; q++){
					mvwaddstr(win, tyoffset+q, txoffset, "       ");
				}

                int xoffset = 4+8*i;
                int yoffset = 2+4*j;

                char valstr[5];
                sprintf(valstr,"%d", grid[i][j]);

                mvwaddstr(win, yoffset, xoffset, valstr);

                wattroff(win, COLOR_PAIR(color_pair));

            }
        }
    }
}

void draw_board(WINDOW* win){
    for(size_t i = 0; i < nrows; i++){
        mvwaddstr(win, i, 0, gameboard[i]);
    }
}

void add_random(){
    int isset = false; 
    srand(time(0));
    // 33% chance of getting a 4
    int num = rand()%3;
    num = num == 0 ? 4 : 2;
    while(!isset){
        int x = rand()%4;
        int y = rand()%4;
        if(grid[x][y] == 0){
            grid[x][y] = num;
            isset = true;
        }
    }
}

void tilt_up(){
    for(int j = 0; j < 4; j++){
        for(int i = 0; i < 4; i++){
            if(grid[i][j] != 0){
                for(int p = j; p > 0; p--){
                    //move value up
                    if(grid[i][p-1] == 0){
                        grid[i][p-1] = grid[i][p];
                        grid[i][p] = 0;
                    }
                    // merge value with tile above it
                    else if(grid[i][p-1] == grid[i][p]){
                        grid[i][p-1] += grid[i][p];
                        grid[i][p] = 0;
                    }
                }
            }
        }
    }
}

void tilt_down(){
    for(int j = 3; j >= 0; j--){
        for(int i = 0; i < 4; i++){
            if(grid[i][j] != 0){
                for(int p = j; p < 3; p++){
                    //move value up
                    if(grid[i][p+1] == 0){
                        grid[i][p+1] = grid[i][p];
                        grid[i][p] = 0;
                    }
                    // merge value with tile above it
                    else if(grid[i][p+1] == grid[i][p]){
                        grid[i][p+1] += grid[i][p];
                        grid[i][p] = 0;
                    }
                }
            }
        }
    }
}

void tilt_left(){
    for(int i = 0; i < 4; i++){
        for(int j = 3; j >= 0; j--){
            if(grid[i][j] != 0){
                for(int p = i; p > 0; p--){
                    //move value up
                    if(grid[p-1][j] == 0){
                        grid[p-1][j] = grid[p][j];
                        grid[p][j] = 0;
                    }
                    // merge value with tile above it
                    else if(grid[p-1][j] == grid[p][j]){
                        grid[p-1][j] += grid[p][j];
                        grid[p][j] = 0;
                    }
                }
            }
        }
    }
}

void tilt_right(){
    for(int i = 3; i >= 0; i--){
        for(int j = 3; j >= 0; j--){
            if(grid[i][j] != 0){
                for(int p = i; p < 3; p++){
                    //move value up
                    if(grid[p+1][j] == 0){
                        grid[p+1][j] = grid[p][j];
                        grid[p][j] = 0;
                    }
                    // merge value with tile above it
                    else if(grid[p+1][j] == grid[p][j]){
                        grid[p+1][j] += grid[p][j];
                        grid[p][j] = 0;
                    }
                }
            }
        }
    }
}


