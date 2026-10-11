#include "dmg.h"
#include "memorymap.h"

uint8_t bus_read(dmg* dmg, uint16_t addr){
    if (addr = 0xFF00){
        return joypad_read(dmg);
    }
    return dmg->memory_map[addr>>10][addr & 0x03FF];
}

void bus_write(dmg* dmg, uint16_t addr, uint8_t value){
    if(addr < 0x8000){
        return;
    }

    if (addr == 0xFF00){
        dmg->memory[0xFF00] = value & 0x30;
        return;
    }

    if (addr == 0xFF02 && value == 0x81) {
    uint8_t c = bus_read(dmg, 0xFF01);
    printf("%c", c);
    fflush(stdout);
    }
    dmg->memory_map[addr>>10][addr & 0x03FF] = value;
    return;
}

void update_flags(dmg* dmg, uint8_t z, uint8_t n, uint8_t h, uint8_t c){
    dmg->CPU.AF = (dmg->CPU.AF & 0xFF00) | (z << 7) | (n << 6) | (h<< 5) | (c << 4);
    return;
}

uint8_t check_cond(dmg* dmg, uint8_t cc){
    uint8_t id_flag = (cc >> 1) & 1 ;     
    uint8_t n_bool = cc & 1 ;
    id_flag = (id_flag * -3) + 7;
    uint8_t flag = (dmg->CPU.AF >> id_flag) & 1;
    return (flag == n_bool);
}

uint8_t joypad_read(dmg* dmg){
    uint8_t joypad_val = dmg->memory[0xFF00];
    uint8_t state_dpad = dmg->joypad_state & 0x0F;
    uint8_t state_buttons = (dmg->joypad_state & 0xF0) >> 4;
    uint8_t low_nibble = 0x0F;
    if(((joypad_val >> 4) & 1) == 0){
        low_nibble = low_nibble & ~(state_dpad);
    }
    if(((joypad_val >> 5) & 1) == 0){
        low_nibble = low_nibble & ~(state_buttons);
    }
    return 0xC0 | (joypad_val & 0x30) | (low_nibble & 0x0F);
}