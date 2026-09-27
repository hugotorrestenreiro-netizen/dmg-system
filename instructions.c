#include "instructions.h"


void NOP(dmg* dmg){
    dmg->CPU.PC ++;
}

void LD_8bit(dmg* dmg){
    uint8_t yyy = (dmg->memory[dmg->CPU.PC] & 0x38) >> 3;
    uint8_t zzz = (dmg->memory[dmg->CPU.PC] & 0x07);
}