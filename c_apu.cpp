#include "c_apu.h"
#include "c_apu.h"
#include "Gb_Snd_Emu-main/Basic_Gb_Apu.h"
#include <SDL2/SDL.h>

static Basic_Gb_Apu apu;
static int16_t audio_buffer[4096];
static SDL_AudioDeviceID audio_dev = 0;

extern "C" void apu_init(uint32_t sample_rate, uint32_t device_id) {
    apu.set_sample_rate(sample_rate);
    audio_dev = (SDL_AudioDeviceID)device_id;
}

extern "C" void apu_write(uint16_t addr, uint8_t val) {
    apu.write_register(addr, val);
}

extern "C" uint8_t apu_read(uint16_t addr) {
    return apu.read_register(addr);
}

extern "C" void apu_end_frame(uint32_t t_cycles) {
    (void)t_cycles;
    apu.end_frame();

    long count = apu.read_samples(audio_buffer, 4096);
    if (count > 0 && audio_dev != 0) {
        if (SDL_GetQueuedAudioSize(audio_dev) < 16384) {
            SDL_QueueAudio(audio_dev, audio_buffer, count * sizeof(int16_t));
        }
    }
}