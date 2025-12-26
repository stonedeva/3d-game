all:
	cc -o game *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -I include
debug:
	cc -o game *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -g -I include
