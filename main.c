#include <dirent.h>
#include <string.h>
#include "dmg.h"
#include "render.h"
#include "instructions.h"
#include "ppu.h"
#include "c_apu.h"

int main(int argc, char *argv[]){

    (void)argc;
    (void)argv;

    //  FIND ROM DIRECTORY

    DIR* rom_directory = opendir("roms");
    if(rom_directory == NULL){
        printf("Could not find 'roms' directory.\n");
        return 1;
    }
    char* extension;
    char file_loc_array[256] = "";
    struct dirent* file;

    while((file = readdir(rom_directory)) != NULL){
        extension = strrchr(file->d_name , '.');
        if(extension && strcmp(extension, ".gb") == 0){
            snprintf(file_loc_array, sizeof(file_loc_array), "roms/%s", file->d_name);
            closedir(rom_directory);
            break;
        }
    }

    if (file_loc_array[0] == '\0') {
    printf("No rom found\n");
    closedir(rom_directory);
    return 1;
    }

    char* file_loc = file_loc_array;

    int capped_fps = 1;

    //  SDL INIT (RENDERING AND AUDIO)

    SDL_Window* window = open_window();
    SDL_Renderer* renderer = initiate_renderer(window);
    SDL_Event event;

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioSpec wanted;
    SDL_zero(wanted);
    wanted.freq = 44100;
    wanted.format = AUDIO_S16SYS;
    wanted.channels = 2;
    wanted.samples = 1024;
 
    SDL_AudioDeviceID audio_dev = SDL_OpenAudioDevice(NULL, 0, &wanted, NULL, 0);
    if (audio_dev == 0) {
        printf("Erreur Ouverture Audio SDL: %s\n", SDL_GetError());
    }
    int audio_started = 0;

    apu_init(44100, audio_dev);
    //  DMG INITIALIZATION

    dmg* dmg;
    dmg = initiate_dmg();

    if (!dmg) {
    return 1;
    }

    SDL_Texture* texture = create_ppu_texture(renderer);

    load_rom(dmg, file_loc);

    if (dmg->MBC.rom_buffer == NULL) {
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
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB){
                    capped_fps = !capped_fps;
                }
            }
            apu_end_frame(70224);
            if(!audio_started && audio_dev != 0 && SDL_GetQueuedAudioSize(audio_dev) >= 8192){
                SDL_PauseAudioDevice(audio_dev, 0);
                audio_started = 1;
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

    if (audio_dev != 0) {
        SDL_CloseAudioDevice(audio_dev);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}