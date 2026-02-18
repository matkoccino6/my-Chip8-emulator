CFLAGS = -std=c2x -Wall -Wextra -Werror -pedantic
PWD := $(shell pwd)
all: 
	gcc $(CFLAGS) src/*.c -o chip8 `pkg-config --cflags --libs sdl2`