#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

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

uint8_t instruction_table(dmg* dmg, uint8_t opcode);
uint8_t x_block(dmg* dmg, uint8_t opcode, uint8_t x);

uint8_t block0_dispatch_z0(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z1(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z2(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z3(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z4(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z5(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z6(dmg* dmg, uint8_t opcode);
uint8_t block0_dispatch_z7(dmg* dmg, uint8_t opcode);

uint8_t block3_z3_dispatch_y0(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y1(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y2(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y3(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y4(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y5(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y6(dmg* dmg, uint8_t opcode);
uint8_t block3_z3_dispatch_y7(dmg* dmg, uint8_t opcode);

uint8_t block3_dispatch_z0(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z1(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z2(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z3(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z4(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z5(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z6(dmg* dmg, uint8_t opcode);
uint8_t block3_dispatch_z7(dmg* dmg, uint8_t opcode);


uint8_t block3_CB_dispatch_x0(dmg* dmg, uint8_t opcode);
uint8_t block3_CB_dispatch_x1(dmg* dmg, uint8_t opcode);
uint8_t block3_CB_dispatch_x2(dmg* dmg, uint8_t opcode);
uint8_t block3_CB_dispatch_x3(dmg* dmg, uint8_t opcode);

#endif