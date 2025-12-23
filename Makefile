all:
	cc -o game *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lm -I include
debug:
	cc -o game *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lm -g -I include
