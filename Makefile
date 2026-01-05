all:
	cc -o game *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -I include
browser:
	emcc *.c -o browser/game.html -DTARGET_BROWSER -Iinclude \
						  -s USE_SDL=2 \
						  -s USE_SDL_TTF=2 \
						  -s USE_SDL_IMAGE=2 \
						  -s SDL2_IMAGE_FORMATS='["png"]' \
						  -s USE_SDL_MIXER=2 \
						  --preload-file res \
						  -O2
debug:
	cc -o game *.c -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lm -g -I include
