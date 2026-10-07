#include "dmg.h"
#include "memorymap.h"

uint8_t bus_read(dmg* dmg, uint16_t addr){
    return dmg->memory_map[addr>>10][addr & 0x03FF];
}

void bus_write(dmg* dmg, uint16_t addr, uint8_t value){
    if(addr == 0xFF01){
        printf("%c", value);
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