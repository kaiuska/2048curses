
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
NULL
};

int values[4][4] = {};

void draw_board(WINDOW* win);
void draw_values(WINDOW* win, int use_color);

void tilt_up();
void tilt_down();
void tilt_left();
void tilt_right();

void add_random(int seed);




int main(void){
    initscr();
    int has_color = has_colors();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    WINDOW* gamewin = newwin(rows, cols, 1, 1);

    curs_set(0);
    cbreak(); 
    noecho(); 
    keypad(gamewin, TRUE);

    if (!has_colors()) {
        mvwprintw(gamewin, 0, 0, "Your terminal does not support color.");
        wrefresh(gamewin);
        wgetch(gamewin);
        wclear(gamewin);
    } else {

        start_color();

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
    }

    int ch;
    short quit = false;

    add_random(time(0));
    add_random(time(0) + 0x2048);

    draw_board(gamewin);
    draw_values(gamewin, has_color);
    wrefresh(gamewin);

    while(quit == false){

        // if user hasn't provided input
        ch = wgetch(gamewin);

        if(ch != ERR && ch != KEY_RESIZE) {

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
            
            add_random(time(0));

            wclear(gamewin);
        }

        wrefresh(gamewin);
        draw_board(gamewin);
        draw_values(gamewin, has_color);
    }

    delwin(gamewin);
    endwin();

    return EXIT_SUCCESS;
}


// draw the colored tiles and cell numbers over top of the gameboard
void draw_values(WINDOW* win, int use_color){
    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){

            if(values[i][j] != 0){

                int color_pair = log(values[i][j]) / log(2);
                
                if (use_color)
                    wattron(win, COLOR_PAIR(color_pair));

                int txoffset = 1+8*i;
				int tyoffset = 1+4*j;

				for(size_t q = 0; q < 3; q++){
					mvwaddstr(win, tyoffset+q, txoffset, "       ");
				}

                int xoffset = 4+8*i;
                int yoffset = 2+4*j;

                char valstr[6];
                sprintf(valstr,"%d", values[i][j]);

                mvwaddstr(win, yoffset, xoffset, valstr);

                if (use_color)
                    wattroff(win, COLOR_PAIR(color_pair));

            }
        }
    }
}

// draw the gameboard
void draw_board(WINDOW* win){
    for(size_t i = 0; gameboard[i] != NULL; i++){
        mvwaddstr(win, i, 0, gameboard[i]);
    }
}

// randomly place a tile
void add_random(int seed){
    int isset = false; 
    srand(seed);
    // 25% chance of getting a 4
    int num = rand()%4;
    num = num == 0 ? 4 : 2;
    while(!isset){
        int x = rand()%4;
        int y = rand()%4;
        if(values[x][y] == 0){
            values[x][y] = num;
            isset = true;
        }
    }
}

void tilt_up(){
    for(int j = 0; j < 4; j++){
        for(int i = 0; i < 4; i++){
            if(values[i][j] != 0){
                for(int p = j; p > 0; p--){
                    //move value up
                    if(values[i][p-1] == 0){
                        values[i][p-1] = values[i][p];
                        values[i][p] = 0;
                    }
                    // merge value with tile above it
                    else if(values[i][p-1] == values[i][p]){
                        values[i][p-1] += values[i][p];
                        values[i][p] = 0;
                    }
                }
            }
        }
    }
}

void tilt_down(){
    for(int j = 3; j >= 0; j--){
        for(int i = 0; i < 4; i++){
            if(values[i][j] != 0){
                for(int p = j; p < 3; p++){
                    //move value up
                    if(values[i][p+1] == 0){
                        values[i][p+1] = values[i][p];
                        values[i][p] = 0;
                    }
                    // merge value with tile above it
                    else if(values[i][p+1] == values[i][p]){
                        values[i][p+1] += values[i][p];
                        values[i][p] = 0;
                    }
                }
            }
        }
    }
}

void tilt_left(){
    for(int i = 0; i < 4; i++){
        for(int j = 3; j >= 0; j--){
            if(values[i][j] != 0){
                for(int p = i; p > 0; p--){
                    //move value up
                    if(values[p-1][j] == 0){
                        values[p-1][j] = values[p][j];
                        values[p][j] = 0;
                    }
                    // merge value with tile above it
                    else if(values[p-1][j] == values[p][j]){
                        values[p-1][j] += values[p][j];
                        values[p][j] = 0;
                    }
                }
            }
        }
    }
}

void tilt_right(){
    for(int i = 3; i >= 0; i--){
        for(int j = 3; j >= 0; j--){
            if(values[i][j] != 0){
                for(int p = i; p < 3; p++){
                    //move value up
                    if(values[p+1][j] == 0){
                        values[p+1][j] = values[p][j];
                        values[p][j] = 0;
                    }
                    // merge value with tile above it
                    else if(values[p+1][j] == values[p][j]){
                        values[p+1][j] += values[p][j];
                        values[p][j] = 0;
                    }
                }
            }
        }
    }
}


