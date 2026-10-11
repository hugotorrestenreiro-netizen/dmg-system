#include "render.h"

SDL_Window* open_window(){

    SDL_Window* window = NULL;

    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("SDL initialization error.\n");
        return NULL;
    }

    int screen_h = 144;
    int screen_w = 160;

    window = SDL_CreateWindow("DMG SYSTEM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_w*3, screen_h*3, SDL_WINDOW_RESIZABLE);

    if(!window){
        printf("Window initialization error.\n");
        SDL_Quit();
        return NULL;
    }
    return window;
}

SDL_Renderer* initiate_renderer(SDL_Window* window){
    SDL_Renderer* renderer = NULL;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        printf("Rendering initialization error.\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL ;
    }
    return renderer;
}

void handle_key_event(dmg* dmg, SDL_Keycode key, uint8_t state) {
    dmg_key_t key_input;
    switch (key) {

        case UP_input: 
            key_input = BUTTON_UP;
            break; 
        case DOWN_input: 
            key_input = BUTTON_DOWN;
            break; 
        case LEFT_input:
            key_input = BUTTON_LEFT;
            break; 
        case RIGHT_input:
            key_input = BUTTON_RIGHT;
            break; 

        case A_input:
            key_input = BUTTON_A;
            break; 
        case B_input:
            key_input = BUTTON_B;
            break; 
        case START_input:
            key_input = BUTTON_START;
            break; 
        case SELECT_input:
            key_input = BUTTON_SELECT;
            break;  

        default: return;
    }
    state_dispatch_t state_dispatch[2] = {key_up, key_down};
    state_dispatch[state](dmg, key_input);
    return;
}