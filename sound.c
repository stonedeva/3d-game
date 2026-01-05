#include "./sound.h"


Mix_Chunk* sounds[SOUND_COUNT] = {0};

void sound_init()
{
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
	fprintf(stderr, "ERROR: MixOpenAudio(): Failed to open sound: %s\n", Mix_GetError());
	Mix_Quit();
	SDL_Quit();
	exit(1);
    }
    Mix_AllocateChannels(SOUND_CHANNELS);

    sound_load(SOUND_MENU_NAVIGATE, "./res/sounds/menu_navigate.wav");
    sound_load(SOUND_MENU_SELECT, "./res/sounds/menu_select.wav");
    sound_load(SOUND_WALL_DESTROY, "./res/sounds/wall_destroy.wav");
    sound_load(SOUND_TNT_EXPLOSION, "./res/sounds/explosion.wav");
    sound_load(SOUND_PICKUP_KEY, "./res/sounds/coin.wav");
    sound_load(SOUND_PLAYER_DAMAGE, "./res/sounds/player_damage.wav");
    sound_load(SOUND_TIMEKIT, "./res/sounds/timekit.wav");
    sound_load(SOUND_WRONG, "./res/sounds/wrong.wav");
    sound_load(SOUND_DOOR_OPEN, "./res/sounds/open_door.wav");
    sound_load(SOUND_LEVEL_ENTRANCE, "./res/sounds/level_entrance.wav");
    sound_load(SOUND_EXPLOSION, "./res/sounds/explosion.wav");
}

void sound_load(Sound sound_id, char* file_path)
{
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

void sound_cleanup()
{
    for (int i = 0; i < SOUND_COUNT; i++) {
	Mix_FreeChunk(sounds[i]);
    }
    Mix_CloseAudio();
    Mix_Quit();
}
