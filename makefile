

CC= gcc
LIBS= -lncurses -lm

2048curses: src/2048curses.c
	$(CC) $^ $(LIBS) -o $@

clean:
	rm 2048curses
