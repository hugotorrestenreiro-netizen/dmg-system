#include "render.h"

static const uint32_t palette[4] = {
    0x809810FF, // 0 : Vert olive mat (fond doux)
    0x486810FF, // 1 : Vert moyen (bien démarqué du fond)
    0x204010FF, // 2 : Vert sombre
    0x081808FF  // 3 : Noir vert très profond
};

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

    SDL_RenderSetLogicalSize(renderer, 160, 144);

    return renderer;
}

SDL_Texture* create_ppu_texture(SDL_Renderer* renderer) {
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        160,
        144
    );

    if (!texture) {
        printf("Texture creation error: %s\n", SDL_GetError());
    }

    return texture;
}

void render_frame(SDL_Renderer* renderer, SDL_Texture* texture, uint8_t display[]) {
    uint32_t pixels[160 * 144];

    for (int i = 0; i < 160 * 144; i++) {
        uint8_t color_id = display[i] & 0x03;
        pixels[i] = palette[color_id];
    }

    SDL_UpdateTexture(texture, NULL, pixels, 160 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
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
    if (state <= 1) {
        state_dispatch_t state_dispatch[2] = {key_up, key_down};
        state_dispatch[state](dmg, key_input);
    }
    return;
}