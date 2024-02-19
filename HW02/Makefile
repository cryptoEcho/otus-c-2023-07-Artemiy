CC=gcc
CFLAGS=-g -Wall -Wextra -Wpedantic -std=c11

all: rus2utf

rus2utf: rus2utf.c utils.c
	$(CC) $(CFLAGS) -o rus2utf rus2utf.c utils.c

clean:
	rm -f rus2utf *.o
