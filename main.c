#include "dmg.h"
#include "render.h"
#include "instructions.h"

int main(int argc, char *argv[]){

    (void)argc;
    (void)argv;

    char* file_loc = "rom/example.gb";

    uint32_t START ,DT = 16;

    SDL_Window* window = open_window();
    SDL_Renderer* renderer = initiate_renderer(window);
    SDL_Event event;


    dmg* dmg;
    dmg = initiate_dmg();

    if (!window || !renderer || !dmg) {
    return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

    uint8_t* rom_buffer = load_rom(dmg, file_loc);

    if (rom_buffer == NULL) {
        printf("Rom loading error");
        return 1;
    }

    int running = 1;

    dmg->CPU.cycle = 0;

    while(running){

        while(SDL_PollEvent(&event)){
            //Inputs
            if(event.type == SDL_QUIT){
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN) {
                ;
            } 
            else if (event.type == SDL_KEYUP) {
                ;
            }
        }

        //FETCH DECODE EXECUTE
        //[...]

        //ACCUMULATOR


    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}