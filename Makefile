CFLAGS = -std=c23 -Wall -Wextra -Werror -pedantic -lm
PWD := $(shell pwd)
all: 
	gcc $(CFLAGS) src/*.c -o chip8 `sdl2-config --cflags --libs`