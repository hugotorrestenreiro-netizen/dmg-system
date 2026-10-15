#include "dmg.h"
#include "render.h"
#include "instructions.h"
#include "ppu.h"

int main(int argc, char *argv[]){

    (void)argc;
    (void)argv;

    char* file_loc = "tetris.gb";
    int capped_fps = 1;



    SDL_Window* window = open_window();
    SDL_Renderer* renderer = initiate_renderer(window);
    SDL_Event event;


    dmg* dmg;
    dmg = initiate_dmg();

    if (!dmg) {
    return 1;
    }

    SDL_Texture* texture = create_ppu_texture(renderer);

    uint8_t* rom_buffer = load_rom(dmg, file_loc);

    if (rom_buffer == NULL) {
        printf("Rom loading error");
        return 1;
    }

    int running = 1;

    dmg->CPU.cycle = 0;
    uint32_t cycle_old;
    uint32_t delta_cycle;
    uint32_t start;
    uint32_t frame_time;

    while(running){
        
        start = SDL_GetTicks();
        //FETCH DECODE EXECUTE
        cycle_old = dmg->CPU.cycle;
        cpu_step(dmg);
        delta_cycle = dmg->CPU.cycle - cycle_old;
        
        timer_step(dmg, delta_cycle) ;
        ppu_step(dmg, delta_cycle);
        //ACCUMULATOR

        if(dmg->CPU.cycle >= 17556){
            while(SDL_PollEvent(&event)){
                //Inputs
                if(event.type == SDL_QUIT){
                    running = 0;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if(event.key.repeat == 0) {
                    handle_key_event(dmg, event.key.keysym.sym, 1);
                    }
                } 
                else if (event.type == SDL_KEYUP) {
                    handle_key_event(dmg, event.key.keysym.sym, 0);
                }
                if (event.type == SDL_KEYUP && capped_fps == 0){
                    capped_fps = 1;
                }
                else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB){
                    capped_fps = 0;
                }
            }
            render_frame(renderer, texture, dmg->PPU.display);
            dmg->CPU.cycle -= 17556;
            //dmg->PPU.cycle -= 17556*4;

            frame_time = SDL_GetTicks() - start;

            if(frame_time < 16 && capped_fps){
                SDL_Delay(16 - frame_time);
            }
        }
    }

    free(dmg);

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}