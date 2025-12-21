all:
	cc -o game *.c -lSDL2 -lSDL2_image -lm -I include
debug:
	cc -o game *.c -lSDL2 -lSDL2_image -lm -g -I include
