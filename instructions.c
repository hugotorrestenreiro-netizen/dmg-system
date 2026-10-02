#include "instructions.h"

//opcodes are here in binary (more confonrtable to handle)

//BLOCK x = 00

void NOP(dmg* dmg){
    dmg->CPU.PC ++;     //NOP opcode = 0
}

void LD_r16imm16(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;                        //LD r16 imm16  opcode: 00 rr 0 001 (x= 00, y= rr0, zzz=001)
    uint16_t imm16 = (((bus_read(dmg, (dmg->CPU.PC)+2)) << 8) | bus_read(dmg, (dmg->CPU.PC)+1));
    *reg16bit_index(dmg, rr) = imm16;
    dmg->CPU.PC += 3;
    return;
}

void LD_r16mem_a(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;                        //LD [r16mem] a  opcode: 00 rr 0 010 (x= 00, y= rr0, zzz=010)
    uint16_t reg16addr = reg16bit_mem_index(dmg, rr);
    bus_write(dmg, reg16addr, get_reg8bit_index(dmg, 7));
    dmg->CPU.PC ++;
    return;
}

void LD_a_r16mem(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;                        //LD a [r16mem]  opcode: 00 rr 1 010 (x= 00, y= rr1, zzz=010)
    uint16_t reg16addr = reg16bit_mem_index(dmg, rr);
    set_reg8bit_index(dmg, 7, bus_read(dmg, reg16addr));
    dmg->CPU.PC ++;
    return;
}

void LD_imm16mem_sp(dmg* dmg){
    uint16_t imm16 = (((bus_read(dmg, (dmg->CPU.PC)+2)) << 8) | bus_read(dmg, (dmg->CPU.PC)+1)); //LD [imm16] sp opcode: 00 001 000 (x= 00, y= 001, zzz=000)
    bus_write(dmg, imm16, (dmg->CPU.SP & 0x00FF));
    bus_write(dmg, imm16+1, (dmg->CPU.SP & 0xFF00) >> 8);
    dmg->CPU.PC+=3;
    return;
}

void inc_r16(dmg* dmg){                                                //inc r16 opcode: 00 rr0 011 (x=00, y=rr0, z=011)
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;
    *reg16bit_index(dmg, rr)= *reg16bit_index(dmg, rr) + 1;
    dmg->CPU.PC++;
    return;
}

void dec_r16(dmg* dmg){                                                //dec r16 opcode: 00 rr1 011 (x=00, y=rr1, z=011)
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;
    *reg16bit_index(dmg, rr)= *reg16bit_index(dmg, rr) - 1;
    dmg->CPU.PC++;
    return;
}

void add_hlr16(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;
    uint16_t r16 = *reg16bit_index(dmg, rr);
    uint32_t res = dmg->CPU.HL + r16 ;                //add hl r16 opcode: 00 rr1 001 (x= 00, y= rr1, z= 001)

    //Extract flags
    uint8_t z = (dmg->CPU.AF >> 7 ) & 1;
    uint8_t h = (((dmg->CPU.HL & 0x0FFF) + (r16 & 0x0FFF)) >> 12) & 1;
    uint8_t c = (res >> 16) & 1;

    //Update flags 
    dmg->CPU.AF = (dmg->CPU.AF & 0xFF00) | (z << 7) | (0 << 6) | (h<< 5) | (c << 4);

    dmg->CPU.HL = (uint16_t) res;

    dmg->CPU.PC++;
    return;
}

void inc_r8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;        //inc r8 opcode: 00 rr0 100 (x= 00, y= rrr, z= 100)
    uint8_t val = get_reg8bit_index(dmg, rr);
    set_reg8bit_index(dmg, rr, val + 1);                           //memo: ATTENTION GERER LES FLAGS Flags: Z 0 8-bit -
    dmg->CPU.PC++;
    return;
}                                                              //Je suis foutu pour les flags de ces deux fonctions :( :(

void dec_r8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;         //inc r8 opcode: 00 rrr 101 (x= 00, y= rrr, z= 101)
    uint8_t val = get_reg8bit_index(dmg, rr);
    set_reg8bit_index(dmg, rr, val - 1);                           //memo: ATTENTION GERER LES FLAGS Flags: Z 0 8-bit -
    dmg->CPU.PC++;
    return;
}

void LD_r8imm8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;          //LD r8 imm8 opcode: 00 rrr 110 (x= 00, y= rrr, z= 110)
    uint8_t imm8 = bus_read(dmg, dmg->CPU.PC + 1);
    set_reg8bit_index(dmg, rr, imm8);
    dmg->CPU.PC+=2;
    return;
}

void rlca(dmg* dmg){
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint8_t bit7 = reg_a >> 7;
    uint8_t c = bit7;
    update_flags(dmg, 0, 0, 0, c);
    uint8_t res = (reg_a << 1) | bit7;
    set_reg8bit_index(dmg, 7, res);
    dmg->CPU.PC++;
    return;
}

//BLOCK x = 01

void LD_r8r8(dmg* dmg){
    uint8_t yyy = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;
    uint8_t zzz = (bus_read(dmg, dmg->CPU.PC) & 0x07);            //LD r8 r8'  opcode: 01 rrr rrr (x= 00, y= rrr, zzz= rrr)
    set_reg8bit_index(dmg, yyy, get_reg8bit_index(dmg, zzz));        //1 M cycle
    dmg->CPU.PC++;
    return;
}

//BLOCK x = 10

//BLOCK x = 11

//0xCB