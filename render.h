#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include <SDL2/SDL.h>
#include "dmg.h"

#define UP_input     SDLK_UP
#define DOWN_input   SDLK_DOWN
#define LEFT_input   SDLK_LEFT
#define RIGHT_input  SDLK_RIGHT
#define A_input      SDLK_a
#define B_input      SDLK_b
#define START_input  SDLK_w
#define SELECT_input SDLK_x

SDL_Window* open_window();
SDL_Renderer* initiate_renderer(SDL_Window* window);
void handle_key_event(dmg* dmg, SDL_Keycode key, uint8_t state);

#endif