#include "instructions.h"

//opcodes are here in binary (more confonrtable to handle)
//if any questions about variable names, go to PanDocs: "https://gbdev.io/pandocs/About.html"
//instructions return the number of cpu cycles

//BLOCK x = 00

uint8_t NOP(dmg* dmg){
    dmg->CPU.PC ++;     //NOP opcode = 0
    return 1;
}

uint8_t LD_r16imm16(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;                        //LD r16 imm16  opcode: 00 rr 0 001 (x= 00, y= rr0, zzz=001)
    uint16_t imm16 = (((bus_read(dmg, (dmg->CPU.PC)+2)) << 8) | bus_read(dmg, (dmg->CPU.PC)+1));
    *reg16bit_index(dmg, rr) = imm16;
    dmg->CPU.PC += 3;
    return 3;
}

uint8_t LD_r16mem_a(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;                        //LD [r16mem] a  opcode: 00 rr 0 010 (x= 00, y= rr0, zzz=010)
    uint16_t reg16addr = reg16bit_mem_index(dmg, rr);
    bus_write(dmg, reg16addr, get_reg8bit_index(dmg, 7));
    dmg->CPU.PC ++;
    return 2;
}

uint8_t LD_a_r16mem(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;                        //LD a [r16mem]  opcode: 00 rr 1 010 (x= 00, y= rr1, zzz=010)
    uint16_t reg16addr = reg16bit_mem_index(dmg, rr);
    set_reg8bit_index(dmg, 7, bus_read(dmg, reg16addr));
    dmg->CPU.PC ++;
    return 2;
}

uint8_t LD_imm16mem_sp(dmg* dmg){
    uint16_t imm16 = (((bus_read(dmg, (dmg->CPU.PC)+2)) << 8) | bus_read(dmg, (dmg->CPU.PC)+1)); //LD [imm16] sp opcode: 00 001 000 (x= 00, y= 001, zzz=000)
    bus_write(dmg, imm16, (dmg->CPU.SP & 0x00FF));
    bus_write(dmg, imm16+1, (dmg->CPU.SP & 0xFF00) >> 8);
    dmg->CPU.PC+=3;
    return 5;
}

uint8_t inc_r16(dmg* dmg){                                                //inc r16 opcode: 00 rr0 011 (x=00, y=rr0, z=011)
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;
    *reg16bit_index(dmg, rr)= *reg16bit_index(dmg, rr) + 1;
    dmg->CPU.PC++;
    return 2;
}

uint8_t dec_r16(dmg* dmg){                                                //dec r16 opcode: 00 rr1 011 (x=00, y=rr1, z=011)
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;
    *reg16bit_index(dmg, rr)= *reg16bit_index(dmg, rr) - 1;
    dmg->CPU.PC++;
    return 2;
}

uint8_t add_hlr16(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 4;
    uint16_t r16 = *reg16bit_index(dmg, rr);
    uint32_t res = dmg->CPU.HL + r16 ;                //add hl r16 opcode: 00 rr1 001 (x= 00, y= rr1, z= 001)

    //Extract flags
    uint8_t z = (dmg->CPU.AF >> 7 ) & 1;
    uint8_t h = (((dmg->CPU.HL & 0x0FFF) + (r16 & 0x0FFF)) >> 12) & 1;
    uint8_t c = (res >> 16) & 1;

    //Update flags 
    update_flags(dmg, z, 0, h, c);

    dmg->CPU.HL = (uint16_t) res;

    dmg->CPU.PC++;
    return 2;
}

uint8_t inc_r8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;        //inc r8 opcode: 00 rr0 100 (x= 00, y= rrr, z= 100)
    uint8_t val = get_reg8bit_index(dmg, rr);
    uint8_t res = val + 1;
    uint8_t c = (dmg->CPU.AF >> 4) & 1;
    uint8_t val_4bit = (val & 0x0F);
    val_4bit += 1;
    uint8_t h = val_4bit >> 4;
    uint8_t z = !res; //if res = 0, z = 1, else z = 0
    update_flags(dmg, z, 0, h, c); 
    set_reg8bit_index(dmg, rr, res);                           
    dmg->CPU.PC++;
    return 1;
}                                                              //Je suis foutu pour les flags de ces deux fonctions :( :(. upd: PAS DU TOUT EN FAIT LA VIE EST BELLE.

uint8_t dec_r8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;         //inc r8 opcode: 00 rrr 101 (x= 00, y= rrr, z= 101)
    uint8_t val = get_reg8bit_index(dmg, rr);
    uint8_t res = val - 1;
    uint8_t c = (dmg->CPU.AF >> 4) & 1;
    uint8_t h = !(val & 0x0F);
    uint8_t z = !res; //if res = 0, z = 1, else z = 0
    update_flags(dmg, z, 1, h, c); 
    set_reg8bit_index(dmg, rr, res);                           //memo: ATTENTION GERER LES FLAGS Flags: Z 1 8-bit -
    dmg->CPU.PC++;
    return 1;
}

uint8_t LD_r8imm8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;          //LD r8 imm8 opcode: 00 rrr 110 (x= 00, y= rrr, z= 110)
    uint8_t imm8 = bus_read(dmg, dmg->CPU.PC + 1);
    set_reg8bit_index(dmg, rr, imm8);
    dmg->CPU.PC+=2;
    return 2;
}

uint8_t rlca(dmg* dmg){
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint8_t bit7 = reg_a >> 7;
    uint8_t c = bit7;
    update_flags(dmg, 0, 0, 0, c);
    uint8_t res = (reg_a << 1) | bit7;
    set_reg8bit_index(dmg, 7, res);
    dmg->CPU.PC++;
    return 1;
}

//BLOCK x = 01

uint8_t LD_r8r8(dmg* dmg){
    uint8_t yyy = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;
    uint8_t zzz = (bus_read(dmg, dmg->CPU.PC) & 0x07);            //LD r8 r8'  opcode: 01 rrr rrr (x= 00, y= rrr, zzz= rrr)
    set_reg8bit_index(dmg, yyy, get_reg8bit_index(dmg, zzz));        //1 M cycle
    dmg->CPU.PC++;
    return 1;
}

//BLOCK x = 10

//BLOCK x = 11

//0xCB