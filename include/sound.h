#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <SDL2/SDL_mixer.h>

#define SOUND_CHANNELS 32

typedef enum {
    SOUND_MENU_SELECT = 0,
    SOUND_WALL_DESTROY,
    SOUND_TNT_EXPLOSION,
    SOUND_PICKUP_KEY,
    SOUND_PLAYER_DAMAGE,
    SOUND_MEDKIT,
    SOUND_WRONG,
    SOUND_DOOR_OPEN,
    SOUND_COUNT
} Sound;

extern Mix_Chunk* sounds[SOUND_COUNT];


void sound_init();
void sound_load(Sound sound_id, char* file_path);
void sound_play(Sound sound_id);
void sound_cleanup();

#endif // _AUDIO_H_
