#ifndef DMG
#define DMG

#include <stdint.h>

typedef struct cpu
{
    uint16_t AF ;  // Accumulator & FLags (High: A, Low: Flags (bit7:z, bit6:n, bit5:h, bit4:c))
    uint16_t BC ; // BC (High: B, Low: C)
    uint16_t DE ; // DE (High: D, Low: E)
    uint16_t HL ; // HL (High: H, Low: L)
    uint16_t SP ; // SP (stack)
    uint16_t PC ; // PC
};

typedef struct ppu
{
    /* data */
};



typedef struct dmg
{
    cpu* CPU;
    ppu* PPU;
};


#endif