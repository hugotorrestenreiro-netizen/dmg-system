#include "memorymap.h"

uint8_t bus_read(dmg* dmg, uint16_t addr){
    return dmg->memory_map[addr>>10][addr & 0x03FF];
}

void bus_write(dmg* dmg, uint16_t addr, uint8_t value){
    dmg->memory_map[addr>>10][addr & 0x03FF] = value;
    return;
}

void update_flags(dmg* dmg, uint8_t z, uint8_t n, uint8_t h, uint8_t c){
    dmg->CPU.AF = (dmg->CPU.AF & 0xFF00) | (z << 7) | (n << 6) | (h<< 5) | (c << 4);
    return;
}