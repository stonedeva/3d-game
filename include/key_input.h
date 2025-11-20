#ifndef _KEY_INPUT_H_
#define _KEY_INPUT_H_

#include <stdint.h>
#include <stdbool.h>

const uint8_t* state = SDL_GetKeyboardState(0);

bool key_is_pressed(int key);

#endif // _KEY_INPUT_H_
