#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include <stdio.h>
#include <stdlib.h>
#include "dmg.h"
#include "memorymap.h"

typedef uint8_t (*dispatch_t)(dmg* dmg, uint8_t opcode);
typedef uint8_t (*function_dispatch_t)(dmg* dmg);


uint8_t  block0_dispatch(dmg* dmg, uint8_t opcode);
uint8_t  block1_dispatch(dmg* dmg, uint8_t opcode);
uint8_t  block2_dispatch(dmg* dmg, uint8_t opcode);
uint8_t  block3_dispatch(dmg* dmg, uint8_t opcode);

uint8_t block0_dispatch_z0(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z1(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z2(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z3(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z4(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z5(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z6(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z7(dmg* dmg, uint8_t opcode);

#endif