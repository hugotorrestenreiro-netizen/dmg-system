#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include <stdio.h>
#include <stdlib.h>
#include "dmg.h"
#include "memorymap.h"

uint8_t NOP(dmg* dmg);
uint8_t LD_r16imm16(dmg* dmg);
uint8_t LD_r16mem_a(dmg* dmg);
uint8_t LD_a_r16mem(dmg* dmg);
uint8_t LD_imm16mem_sp(dmg* dmg);
uint8_t inc_r16(dmg* dmg);
uint8_t dec_r16(dmg* dmg);
uint8_t add_hlr16(dmg* dmg);
uint8_t inc_r8(dmg* dmg);
uint8_t dec_r8(dmg* dmg);
uint8_t LD_r8imm8(dmg* dmg);
uint8_t rlca(dmg* dmg);
uint8_t LD_r8r8(dmg* dmg);


#endif