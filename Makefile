

build: mato

CC=gcc
CFLAGS=-O3 -g -std=c99

mato: src/*.c src/*.h
	$(CC) $(CFLAGS) src/*.c -o mato -lncurses
