#ifndef MEMORYMAP
#define MEMORYMAP

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct dmg dmg;
uint8_t bus_read(dmg* dmg, uint16_t addr);
void bus_write(dmg* dmg, uint16_t addr, uint8_t value);
void update_flags(dmg* dmg, uint8_t z, uint8_t n, uint8_t h, uint8_t c);
uint8_t check_cond(dmg* dmg, uint8_t cc);

#endif