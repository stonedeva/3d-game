all:
	cc -o game src/*.c -O2 -march=native -ffast-math \
			   -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
			   -lm -I include

windows:
	x86_64-w64-mingw32-gcc src/*.c -o game.exe -I include -lmingw32 \
						   -lSDL2main -lSDL2 \
						   -lSDL2_image -lSDL2_ttf \
						   -lSDL2_mixer -lm

debug:
	cc -o game src/*.c -O2 -march=native -ffast-math \
			   -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer \
			   -lm -g -I include
