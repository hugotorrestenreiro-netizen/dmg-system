#include "instructions.h"

//opcodes are here in binary (more confonrtable to handle)

//BLOCK x = 00

void NOP(dmg* dmg){
    dmg->CPU.PC ++;     //NOP opcode = 0
}

void LD_r16imm16(dmg* dmg){
    uint8_t rr = (dmg->memory[dmg->CPU.PC] & 0x38) >> 4;                        //LD r16 imm16  opcode: 00 rr 0 001 (x= 00, y= rr0, zzz=001)
    uint16_t imm16 = (((dmg->memory[(dmg->CPU.PC)+2]) << 8) | dmg->memory[(dmg->CPU.PC)+1]);
    *reg16bit_index(dmg, rr) = imm16;
    dmg->CPU.PC += 3;
    return;
}

void LD_r16mem_a(dmg* dmg){
    uint8_t rr = (dmg->memory[dmg->CPU.PC] & 0x38) >> 4;                        //LD [r16mem] a  opcode: 00 rr 0 010 (x= 00, y= rr0, zzz=010)
    uint16_t reg16addr = reg16bit_mem_index(dmg, rr);
    dmg->memory[reg16addr] = *reg8bit_index(dmg, 7);
    dmg->CPU.PC ++;
    return;
}

void LD_a_r16mem(dmg* dmg){
    uint8_t rr = (dmg->memory[dmg->CPU.PC] & 0x38) >> 4;                        //LD a [r16mem]  opcode: 00 rr 1 010 (x= 00, y= rr1, zzz=010)
    uint16_t reg16addr = reg16bit_mem_index(dmg, rr);
    *reg8bit_index(dmg, 7) = dmg->memory[reg16addr];
    dmg->CPU.PC ++;
    return;
}

void LD_imm16mem_sp(dmg* dmg){
    uint16_t imm16 = (((dmg->memory[(dmg->CPU.PC)+2]) << 8) | dmg->memory[(dmg->CPU.PC)+1]); //LD [imm16] sp opcode: 00 001 000 (x= 00, y= 001, zzz=000)
    dmg->memory[imm16] = (dmg->CPU.SP & 0x00FF) ;
    dmg->memory[imm16+1] = (dmg->CPU.SP & 0xFF00) >> 8 ;
    dmg->CPU.PC+=3;
    return;
}

//BLOCK x = 01

void LD_r8r8(dmg* dmg){
    uint8_t yyy = (dmg->memory[dmg->CPU.PC] & 0x38) >> 3;
    uint8_t zzz = (dmg->memory[dmg->CPU.PC] & 0x07);            //LD r8 r8'  opcode: 01 rrr rrr (x= 00, y= rrr, zzz= rrr)
    *reg8bit_index(dmg, yyy) = *reg8bit_index(dmg, zzz);        //1 M cycle
    dmg->CPU.PC++;
    return;
}

//BLOCK x = 10

//BLOCK x = 11

//0xCB