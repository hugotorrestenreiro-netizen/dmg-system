#include "memorymap.h"

uint8_t bus_read(dmg* dmg, uint16_t addr){
    return dmg->memory_map[addr>>10][addr & 0x03FF];
}

void bus_write(dmg* dmg, uint16_t addr, uint8_t value){
    dmg->memory_map[addr>>10][addr & 0x03FF] = value;
    return;
}