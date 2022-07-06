CC := gcc
CFLAGS := -g -Wall -Wno-deprecated-declarations -Werror

all: worm

clean:
	rm -rf worm

clean:
	rm -rf worm

worm: worm.c util.c scheduler.c
	$(CC) $(CFLAGS) worm.c util.c scheduler.c -o worm -lncurses

test1: test1.c util.c scheduler.c
	$(CC) $(CFLAGS) test1.c util.c scheduler.c -o test1 -lncurses

test2: test2.c util.c scheduler.c
	$(CC) $(CFLAGS) test2.c util.c scheduler.c -o test2 -lncurses

test3: test3.c util.c scheduler.c
	$(CC) $(CFLAGS) test3.c util.c scheduler.c -o test3 -lncurses

test4: test4.c util.c scheduler.c
	$(CC) $(CFLAGS) test4.c util.c scheduler.c -o test4 -lncurses

