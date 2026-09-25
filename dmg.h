#ifndef DMG
#define DMG

#include <stdint.h>

typedef struct
{
    uint16_t AF ;  // Accumulator & FLags (High: A, Low: Flags (bit7:z, bit6:n, bit5:h, bit4:c))
    uint16_t BC ; // BC (High: B, Low: C)
    uint16_t DE ; // DE (High: D, Low: E)
    uint16_t HL ; // HL (High: H, Low: L)
    uint16_t SP ; // SP (stack)
    uint16_t PC ; // PC
}cpu;

typedef struct
{
    uint8_t diplay[160*144]; //Diplay: 160*144 pixels (from 0 to 3 (there are 4 shades of green))
}ppu;



typedef struct dmg
{
    cpu CPU;
    ppu PPU;
    uint8_t memory[0x10000];
};


#endif