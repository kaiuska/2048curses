#include <curses.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#define HIST_SIZE   32

#define UP          0
#define RIGHT       1
#define DOWN        2
#define LEFT        3

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
#define COLOR_ORANGE    8
#define COLOR_MAROON    9
#define COLOR_LIGHTBLUE 10
#define COLOR_PINK      11
#define COLOR_GOLD      12

const char *title = 
"/----------| 2 0 4 8 |----------\\";
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

void draw_menu(WINDOW* win, int turn_count);
void draw_board(WINDOW* win, int values[][4]);
void draw_values(WINDOW* win, int values[][4], int use_color);

int tilt(int values[][4], int direction);

void add_random(int values[][4]);
int can_move(int values[][4]);
int board_full(int values[][4]);

void copy_values(int dest[][4], int source[][4]);
void reset(int values[][4], int history[][4][4], int *history_top, int *history_count); 
void record_state(int values[][4], int history[][4][4], int *history_top, int *history_count);
void rotate(int ar[][4], int n);
int get_random_int();

int init_color_pairs();


int main(void) 
{
    initscr();

    int has_color = has_colors();
    int rows, cols;

    getmaxyx(stdscr, rows, cols);

    WINDOW* titlewin = newwin(1, cols, 0, 1);
    WINDOW* gamewin = newwin(17, cols, 1, 1);
    WINDOW* infowin = newwin(5, cols, 18, 0);

    curs_set(0);
    cbreak(); 
    noecho(); 
    keypad(gamewin, TRUE);
    
    int values[4][4] = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
    };
    int turn_count = 0;

    int history[HIST_SIZE][4][4] = {};
    int history_top = 0;
    int history_count = 0;

    // initialize colors (if supported)
    if (has_color) {
        init_color_pairs();
        wattron(gamewin, COLOR_PAIR(0));
    } else {
        mvwprintw(gamewin, 9, 0, "Your terminal does not support color.");
        wrefresh(gamewin);
        wgetch(gamewin);
        wclear(gamewin);
    }

    int ch;
    int quit = 0;
    int modified = 1;
    int game_over = 0;
    int reset_board = 0;

    reset(values, history, &history_top, &history_count);
    add_random(values);
    add_random(values);

    draw_board(gamewin, values);
    draw_values(gamewin, values, has_color);
    wrefresh(titlewin);
    wrefresh(gamewin);

    while(quit == 0) {

        // draw title
        mvwaddstr(titlewin, 0, 0, title);
        wrefresh(titlewin);

        draw_menu(infowin, turn_count);
        wrefresh(infowin);

        ch = wgetch(gamewin);

        if (ch != 'u' && ch != 'q' && ch != 'r') {
            record_state(values, history, &history_top, &history_count);
        }

        if(ch != ERR && ch != KEY_RESIZE) {

            modified = 0;
            switch(ch) {
                case KEY_UP:
                    modified = tilt(values, UP);
                    break;
                case KEY_DOWN:
                    modified = tilt(values, DOWN);
                    break;
                case KEY_LEFT:
                    modified = tilt(values, LEFT);
                    break;
                case KEY_RIGHT:
                    modified = tilt(values, RIGHT);
                    break;
                case 'q':
                    quit = 1;
                    break;
                case 'u':
                    if (history_count > 0 && turn_count > 0) {
                        turn_count--;
                        history_count--;
                        history_top = (history_top + HIST_SIZE - 1) % HIST_SIZE;
                        copy_values(values, history[history_top]);
                    } 
                    break;
                case 'r':
                    game_over = 0;
                    reset(values, history, &history_top, &history_count); 
                    add_random(values);
                    add_random(values);
                    turn_count = 0;
                    break;
            }
        }

        if (modified) {
            turn_count++;
            add_random(values);
        }

        if (!can_move(values)){
            game_over = 1;
        }

        wclear(gamewin);

        if (!game_over) {
            draw_board(gamewin, values);
            draw_values(gamewin, values, has_color);
            wclear(titlewin);
            wclear(infowin);
        } else {
            mvwaddstr(gamewin, 9, 11, "GAME OVER");
        }

    }

    delwin(gamewin);
    delwin(titlewin);
    delwin(infowin);
    endwin();

    return EXIT_SUCCESS;
}

// copy gameboard into history
void record_state(int values[][4], int history[][4][4], int *history_top, int *history_count)
{
    if (*history_count < HIST_SIZE) {
        (*history_count)++;
    }
    copy_values(history[*history_top], values);
    *history_top = (*history_top + 1) % (HIST_SIZE);
}

// reset all values is values and history to 0
void reset(int values[][4], int history[][4][4], int *history_top, int *history_count)
{
    for (int i = 0; i < HIST_SIZE; i++) {
        for (int j = 0; j < 4; j++) {
            memset(history[i][j], 0, 4 * sizeof(int));
        }
    }
    for (int i = 0; i < 4; i++) {
        memset(values[i], 0, 4 * sizeof(int));
    }
    *history_top = 0; 
    *history_count = 0;
}

int init_color_pairs()
{
    if (has_colors()) {
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

        return 1;
    } else {
        return 0;
    }
}

// draw the menu
void draw_menu(WINDOW* win, int turn_count)
{
    static char turn_count_str[6] = "";
    wmove(win, 0, 8);
    waddch(win, '('); waddch(win, ACS_LARROW); waddch(win, ')'); waddstr(win, " left   ");
    waddch(win, '('); waddch(win, ACS_RARROW); waddch(win, ')'); waddstr(win, " right\n");
    wmove(win, 1, 8);
    waddch(win, '('); waddch(win, ACS_UARROW); waddch(win, ')'); waddstr(win, " up     ");
    waddch(win, '('); waddch(win, ACS_DARROW); waddch(win, ')'); waddstr(win, " down   ");
    mvwaddstr(win, 2, 8, "(u) undo   (r) reset");

    sprintf(turn_count_str, "turn: %d", turn_count);
    mvwaddstr(win, 3, 14, turn_count_str);
}

// draw the colored tiles and cell numbers over top of the gameboard
void draw_values(WINDOW* win, int values[][4], int use_color) 
{
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {

            if (values[i][j] != 0) {

                int color_pair = log2(values[i][j]);
                
                if (use_color) {
                    wattron(win, COLOR_PAIR(color_pair));
                }

                int tile_x = 1+8*i;
                int tile_y = 1+4*j;

                mvwaddstr(win, tile_y, tile_x, "       ");
                mvwaddstr(win, tile_y+1, tile_x, "       ");
                mvwaddstr(win, tile_y+2, tile_x, "       ");

                char value_str[6];
                sprintf(value_str,"%d", values[i][j]);

                mvwaddstr(win, tile_y+1, tile_x+4 - log10(values[i][j])/2, value_str);

                if (use_color) {
                    wattroff(win, COLOR_PAIR(color_pair));
                }
            }
        }
    }
}

// draw the gameboard
void draw_board(WINDOW* win, int values[][4]) 
{
    for(size_t i = 0; gameboard[i] != NULL; i++) {
        mvwaddstr(win, i, 0, gameboard[i]);
    }
}

// randomly place a tile
void add_random(int values[][4])
{
    int isset = 0; 
    int num = get_random_int()%4;
    num = num == 0 ? 4 : 2;

    if (!board_full(values)) {
        while (!isset) {
            int x = get_random_int()%4;
            int y = get_random_int()%4;
            if (values[x][y] == 0) {
                values[x][y] = num;
                isset = 1;
            }
        }
    }
}

int get_random_int()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec);
    return rand();
}

// copy values from source to dest 
void copy_values(int dest[][4], int source[][4])
{
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            dest[x][y] = source[x][y];
        }
    }
}

// test if the player can move
int can_move(int values[][4])
{
    if (!board_full(values)) {
        return 1;
    } else {
        for (int j = 1; j < 4; j++) {
            for (int i = 1; i < 4; i++) {
                if (values[i-1][j] == values[i][j]) {
                    return 1;
                } else if (values[i][j-1] == values[i][j]) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

// check if board is full
int board_full(int values[][4]) 
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (values[i][j] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

// rotate array clockwise n times
void rotate(int ar[][4], int n)
{
    while (n-- > 0) {
        // transpose array
        for (int i = 0; i < 4; i++) {
            for (int j = i; j < 4; j++) {
                int tmp = ar[j][i];
                ar[j][i] = ar[i][j];
                ar[i][j] = tmp;
            }
        }
        // flip array along x axis
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                int tmp = ar[i][j];
                ar[i][j] = ar[i][3-j];
                ar[i][3-j] = tmp;
            }
        }
    }
}

// slide and merge values
int tilt(int values[][4], int direction)
{
    // rotate board so upward tilt handles tilt in that direction
    rotate(values, direction);

    int modified = 0;
    int merges[4][4] = {};
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            if (values[i][j] != 0) {
                for (int p = j; p > 0; p--) {
                    //move value up
                    if (values[i][p-1] == 0) {
                        values[i][p-1] = values[i][p];
                        values[i][p] = 0;
                        modified = 1;
                    // merge value with tile above it
                    } else if (merges[i][p-1] == 0 && values[i][p-1] == values[i][p]) {
                        values[i][p-1] += values[i][p];
                        values[i][p] = 0;
                        merges[i][p-1] = 1;
                        modified = 1;
                        break;
                    // if the tile above has been merged, move on to the next cell
                    } else if (merges[i][p-1] != 0) {
                        break;
                    }
                }
            }
        }
    }
    // rotate board back into original orientation
    rotate(values, 4 - direction);
    return modified;
}
