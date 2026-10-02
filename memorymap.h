#ifndef MEMORYMAP
#define MEMORYMAP

#include <stdio.h>
#include <stdlib.h>
#include "dmg.h"

uint8_t bus_read(dmg* dmg, uint16_t addr);
void bus_write(dmg* dmg, uint16_t addr, uint8_t value);

#endif