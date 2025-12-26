#include "./sound.h"


Mix_Chunk* sounds[SOUND_COUNT] = {0};

void sound_init()
{
    sound_load(SOUND_MENU_SELECT, "./res/sounds/menu_select.wav");
    sound_load(SOUND_WALL_DESTROY, "./res/sounds/wall_destroy.wav");
    sound_load(SOUND_COIN, "./res/sounds/coin.wav");
}

void sound_load(Sound sound_id, char* file_path)
{
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
	fprintf(stderr, "ERROR: MixOpenAudio(): Failed to open sound: %s\n", Mix_GetError());
	Mix_Quit();
	SDL_Quit();
	exit(1);
    }

    Mix_Chunk* sound = Mix_LoadWAV(file_path);
    if (!sound) {
	fprintf(stderr, "ERROR: MixLoadWAV(): Failed to load sound: %s: %s\n", 
		file_path, Mix_GetError());
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
	exit(1);
    }
    sounds[sound_id] = sound;
}

void sound_play(Sound sound_id)
{
    if (Mix_PlayChannel(-1, sounds[sound_id], 0) < 0) {
	fprintf(stderr, "ERROR: MixPlayChannel(): Failed to play sound: %s\n",
		Mix_GetError());
	exit(1);
    }
}
