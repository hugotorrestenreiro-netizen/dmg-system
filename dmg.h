#ifndef DMG_H
#define DMG_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "memorymap.h"

typedef struct
{
    uint16_t AF ;  // Accumulator (7) & FLags (High: A, Low: Flags (bit7:z, bit6:n, bit5:h, bit4:c))
    uint16_t BC ; // BC (High: B (0), Low: C(1))
    uint16_t DE ; // DE (High: D(2), Low: E(3))
    uint16_t HL ; // HL (High: H(4), Low: L(5))
    uint16_t SP ; // SP (stack)
    uint16_t PC ; // PC 
    uint8_t* reg8bit_map[8]; // 8bit register map
    uint16_t* reg16bit_map[4];
    uint32_t cycle; 
    uint8_t stop;
    uint8_t halt;
}cpu;

typedef struct
{
    uint8_t diplay[160*144]; //Diplay: 160*144 pixels (from 0 to 3 (there are 4 shades of green))
}ppu;



typedef struct dmg
{
    cpu CPU;
    ppu PPU;                            //Principal struct
    uint8_t memory[0x10000];
    uint8_t* memory_map[64];
}dmg;

uint8_t get_reg8bit_index(dmg* dmg, uint8_t input);
void set_reg8bit_index(dmg* dmg, uint8_t input, uint8_t value);
uint16_t* reg16bit_index(dmg* dmg,uint8_t input);
uint16_t reg16bit_mem_index(dmg* dmg,uint8_t input);
dmg* initiate_dmg();
uint8_t* load_rom(dmg* dmg, char* file_loc);

#endif