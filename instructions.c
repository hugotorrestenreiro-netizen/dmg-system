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
    uint8_t reg_a = get_reg8bit_index(dmg, 7);          //rla opcode: 00 000 111 (x= 00, y= 000, z= 111)
    uint8_t bit7 = reg_a >> 7;
    uint8_t c = bit7;
    update_flags(dmg, 0, 0, 0, c);
    uint8_t res = (reg_a << 1) | bit7;
    set_reg8bit_index(dmg, 7, res);
    dmg->CPU.PC++;
    return 1;
}

uint8_t rrca(dmg* dmg){
    uint8_t reg_a = get_reg8bit_index(dmg, 7);          //rrca opcode: 00 001 111 (x= 00, y= 001, z= 111)
    uint8_t bit0 = reg_a & 1;
    uint8_t c = bit0;
    update_flags(dmg, 0, 0, 0, c);
    uint8_t res = (reg_a >> 1) | (bit0 << 7);
    set_reg8bit_index(dmg, 7, res);
    dmg->CPU.PC++;
    return 1;
}

uint8_t rla(dmg* dmg){
    uint8_t reg_a = get_reg8bit_index(dmg, 7);          //rla opcode: 00 010 111 (x= 00, y= 010, z= 111)
    uint8_t c = (dmg->CPU.AF >> 4) & 1;
    uint8_t bit7 = reg_a >> 7;
    update_flags(dmg, 0, 0, 0, bit7);
    uint8_t res = (reg_a << 1) | c;
    set_reg8bit_index(dmg, 7, res);
    dmg->CPU.PC++;
    return 1;
}

uint8_t rra(dmg* dmg){
    uint8_t reg_a = get_reg8bit_index(dmg, 7);          //rra opcode: 00 011 111 (x= 00, y= 011, z= 111)
    uint8_t bit0 = reg_a & 1;
    uint8_t c = (dmg->CPU.AF >> 4) & 1;
    update_flags(dmg, 0, 0, 0, bit0);
    uint8_t res = (reg_a >> 1) | (c << 7);
    set_reg8bit_index(dmg, 7, res);
    dmg->CPU.PC++;
    return 1;
}

uint8_t daa(dmg* dmg){
    uint8_t reg_a = get_reg8bit_index(dmg, 7);          //daa opcode: 00 100 111 (x= 00, y= 100, z= 111)
    //Extract Flags
    uint8_t n = (dmg->CPU.AF >> 6 ) & 1;
    uint8_t h = (dmg->CPU.AF >> 5 ) & 1;
    uint8_t c = (dmg->CPU.AF >> 4 ) & 1;
    //Check if high and low nibble > 9 or flag value (half carry for low nibble and carry for high nibble)
    uint8_t low_nib = h | (!n & ((reg_a & 0x0F) > 9));
    uint8_t high_nib = c | (!n & (reg_a > 99));
    //Assemble high and low nibb, apply it on reg_a
    uint8_t BCD_filter = (high_nib * 0x60) | (low_nib * 0x06);
    reg_a += (1-2*n) * BCD_filter; //1-2*n to check if this is an add or sub
    //Update A Register and flags
    set_reg8bit_index(dmg, 7, reg_a);
    update_flags(dmg, !reg_a, 0, 0, high_nib);
    dmg->CPU.PC ++;
    return 1;
}

uint8_t cpl(dmg* dmg){
    uint8_t a = get_reg8bit_index(dmg, 7);  //opcode: 00 101 111 (x= 00, y= 101, z=111)
    uint8_t c = (dmg->CPU.AF >> 4 ) & 1;
    a = 0xFF - a;
    update_flags(dmg, !a, 1, 1, c);
    set_reg8bit_index(dmg, 7, a);
    dmg->CPU.PC ++;
    return 1;
}

uint8_t scf(dmg* dmg){
    uint8_t z = (dmg->CPU.AF >> 7 ) & 1; //opcode: 00 110 111 (x= 00, y=110, z=111)
    update_flags(dmg, z, 0, 0, 1);
    dmg->CPU.PC ++;
    return 1;
}

uint8_t ccf(dmg* dmg){
    uint8_t z = (dmg->CPU.AF >> 7 ) & 1; //opcode: 00 111 111 (x= 00, y=111, z=111)
    uint8_t c = (dmg->CPU.AF >> 4 ) & 1;
    update_flags(dmg, z, 0, 0, !c);
    dmg->CPU.PC ++;
    return 1;
}

uint8_t jr_imm8(dmg* dmg){ //opcode: 00 011 000 (x= 00, y=011, z=000)
    int8_t imm8 = (int8_t)bus_read(dmg, (dmg->CPU.PC)+1);
    dmg->CPU.PC += imm8 +2;
    return 3;
}

uint8_t jr_cond_imm8(dmg* dmg){ //opcode: 00 1cc 000 (x=00, y=1cc, z=000)
    uint8_t cc = (bus_read(dmg, dmg->CPU.PC) & 0x18) >> 3;
    int8_t imm8 = (int8_t)bus_read(dmg, (dmg->CPU.PC)+1);
    uint8_t cond = check_cond(dmg, cc);
    dmg->CPU.PC += (imm8 * cond) + 2;
    return 2 + cond;
}

uint8_t stop(dmg* dmg){
    dmg->CPU.PC += 2;
    dmg->CPU.stop = 1;
    return 2;
}


//BLOCK x = 01

uint8_t LD_r8r8(dmg* dmg){
    uint8_t yyy = (bus_read(dmg, dmg->CPU.PC) & 0x38) >> 3;
    uint8_t zzz = (bus_read(dmg, dmg->CPU.PC) & 0x07);            //LD r8 r8'  opcode: 01 rrr rrr (x= 00, y= rrr, zzz= rrr)
    set_reg8bit_index(dmg, yyy, get_reg8bit_index(dmg, zzz));        //1 M cycle
    dmg->CPU.PC++;
    return 1;
}

uint8_t halt(dmg* dmg){
    dmg->CPU.halt = 1;
    dmg->CPU.PC ++;
    return 1;
}

//BLOCK x = 2

uint8_t add_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = reg_a + r8 ;                //add a r8 opcode: 10 000 rrr (x= 10, y= 000, z= rrr)
    uint8_t reg_a_4bit = reg_a & 0x0F;
    uint8_t r8_4bit = r8 & 0x0F;
    uint8_t res_4bit = reg_a_4bit + r8_4bit ;

    //Extract flags
    uint8_t h = (res_4bit > 15);
    uint8_t c = (res >> 8) & 1;

    //Update flags 
    update_flags(dmg, !((uint8_t) res), 0, h, c);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;
    return 1 + (rr==6);
}

uint8_t adc_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint8_t c_old = (dmg->CPU.AF >> 4 ) & 1;
    uint16_t res = reg_a + r8 + c_old;                //adc a r8 opcode: 10 001 rrr (x= 00, y= 001, z= rrr)
    uint8_t reg_a_4bit = reg_a & 0x0F;
    uint8_t r8_4bit = r8 & 0x0F;
    uint8_t res_4bit = reg_a_4bit + r8_4bit + c_old;

    //Extract flags
    uint8_t h = (res_4bit > 15);
    uint8_t c_new = (res >> 8) & 1;

    //Update flags 
    update_flags(dmg, !((uint8_t) res), 0, h, c_new);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;
    return 1 + (rr==6);
}

uint8_t sub_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = reg_a - r8 ;                //sub a r8 opcode: 10 010 rrr (x= 10, y= 010, z= rrr)
    uint8_t reg_a_4bit = reg_a & 0x0F;
    uint8_t r8_4bit = r8 & 0x0F;

    //Extract flags
    uint8_t h = (reg_a_4bit < r8_4bit);
    uint8_t c = (reg_a < r8);

    //Update flags 
    update_flags(dmg, !((uint8_t) res), 1, h, c);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;
    return 1 + (rr==6);
}

uint8_t sbc_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t c_old = (dmg->CPU.AF >> 4 ) & 1;
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = reg_a - r8 - c_old;                //sbc a r8 opcode: 10 011 rrr (x= 10, y= 011, z= rrr)
    uint8_t reg_a_4bit = reg_a & 0x0F;
    uint8_t r8_4bit = r8 & 0x0F;

    //Extract flags
    uint8_t h = (reg_a_4bit < (r8_4bit + c_old));
    uint8_t c_new = (reg_a < (r8 + c_old));

    //Update flags 
    update_flags(dmg, !((uint8_t) res), 1, h, c_new);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;
    return 1 + (rr==6);
}

uint8_t and_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = (reg_a & r8) ;                //and a r8 opcode: 10 100 rrr (x= 10, y= 100, z= rrr)


    //Update flags 
    update_flags(dmg, !((uint8_t) res), 0, 1, 0);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;

    return 1 + (rr==6);
}

uint8_t xor_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = (reg_a ^ r8) ;                //xor a r8 opcode: 10 101 rrr (x= 10, y= 101, z= rrr)


    //Update flags 
    update_flags(dmg, !((uint8_t) res), 0, 0, 0);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;

    return 1 + (rr==6);
}

uint8_t or_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = (reg_a | r8) ;                //or a r8 opcode: 10 110 rrr (x= 10, y= 110, z= rrr)


    //Update flags 
    update_flags(dmg, !((uint8_t) res), 0, 0, 0);

    set_reg8bit_index(dmg, 7, res);

    dmg->CPU.PC++;

    return 1 + (rr==6);
}

uint8_t cp_ar8(dmg* dmg){
    uint8_t rr = (bus_read(dmg, dmg->CPU.PC) & 0x07);
    uint8_t r8 = get_reg8bit_index(dmg, rr);
    uint8_t reg_a = get_reg8bit_index(dmg, 7);
    uint16_t res = reg_a - r8 ;                //cp a r8 opcode: 10 111 rrr (x= 10, y= 010, z= rrr)
    uint8_t reg_a_4bit = reg_a & 0x0F;
    uint8_t r8_4bit = r8 & 0x0F;

    //Extract flags
    uint8_t h = (reg_a_4bit < r8_4bit);
    uint8_t c = (reg_a < r8);

    //Update flags 
    update_flags(dmg, !((uint8_t) res), 1, h, c);

    dmg->CPU.PC++;
    return 1 + (rr==6);
}

//BLOCK x = 3

//0xCB

//INSTRUCTION TABLE

uint8_t instruction_table(dmg* dmg, uint8_t opcode){
    uint8_t x = (opcode & 0xC0) >> 6;
    return x_block(dmg, opcode, x);
}

uint8_t x_block(dmg* dmg, uint8_t opcode, uint8_t x){
    static const dispatch_t x_index[4] = {
        block0_dispatch,
        block1_dispatch,
        block2_dispatch,
        block3_dispatch
    };
    return x_index[x](dmg, opcode);
}

uint8_t  block0_dispatch(dmg* dmg, uint8_t opcode){
    uint8_t z = (opcode & 0x07);
    static const dispatch_t block0_z_index[8]={
        block0_dispatch_z0,
        block0_dispatch_z1,
        block0_dispatch_z2,
        block0_dispatch_z3,
        block0_dispatch_z4,
        block0_dispatch_z5,
        block0_dispatch_z6,
        block0_dispatch_z7,
    };
    return block0_z_index[z](dmg,opcode);
}

uint8_t block0_dispatch_z0(dmg* dmg, uint8_t opcode){
    uint8_t y = (opcode & 0x38) >> 3;
    static const function_dispatch_t block0_z0_functions[8]={
        NOP,
        LD_imm16mem_sp,
        stop,
        jr_imm8,
        jr_cond_imm8,
        jr_cond_imm8,
        jr_cond_imm8,
        jr_cond_imm8
    };
    return block0_z0_functions[y](dmg);
}

uint8_t block0_dispatch_z1(dmg* dmg, uint8_t opcode){
    uint8_t y = (opcode & 0x38) >> 3;
    static const function_dispatch_t block0_z1_functions[8]={
        LD_r16imm16, add_hlr16,
        LD_r16imm16, add_hlr16,
        LD_r16imm16, add_hlr16,
        LD_r16imm16, add_hlr16
    };
    return block0_z1_functions[y](dmg);
}

uint8_t block0_dispatch_z2(dmg* dmg, uint8_t opcode){
    uint8_t y = (opcode & 0x38) >> 3;
    static const function_dispatch_t block0_z2_functions[8]={
        LD_r16mem_a, LD_a_r16mem,
        LD_r16mem_a, LD_a_r16mem,
        LD_r16mem_a, LD_a_r16mem,
        LD_r16mem_a, LD_a_r16mem
        
    };
    return block0_z2_functions[y](dmg);
}

uint8_t block0_dispatch_z3(dmg* dmg, uint8_t opcode){
    uint8_t y = (opcode & 0x38) >> 3;
    static const function_dispatch_t block0_z3_functions[8]={
        inc_r16, dec_r16,
        inc_r16, dec_r16,
        inc_r16, dec_r16,
        inc_r16, dec_r16,

    };
    return block0_z3_functions[y](dmg);
}

uint8_t block0_dispatch_z4(dmg* dmg, uint8_t opcode){
    (void)opcode;
    return inc_r8(dmg);
}

uint8_t block0_dispatch_z5(dmg* dmg, uint8_t opcode){
    (void)opcode;
    return dec_r8(dmg);
}

uint8_t block0_dispatch_z6(dmg* dmg, uint8_t opcode){
    (void)opcode;
    return LD_r8imm8(dmg);
}

uint8_t block0_dispatch_z7(dmg* dmg, uint8_t opcode){
    uint8_t y = (opcode & 0x38) >> 3;
    static const function_dispatch_t block0_z7_functions[8]={
        rlca,
        rrca,
        rla,
        rra,
        daa,
        cpl,
        scf,
        ccf
    };
    return block0_z7_functions[y](dmg);
}



uint8_t  block1_dispatch(dmg* dmg, uint8_t opcode){
    uint8_t yz = (opcode & 0x3F);
    static const dispatch_t block1_z_index[64]={
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8,
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8,
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8,
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8,
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, 
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, 
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, 
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, halt, LD_r8r8, 
        LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, LD_r8r8, 
        LD_r8r8
    };
    return block1_z_index[yz](dmg,opcode);
}

uint8_t  block2_dispatch(dmg* dmg, uint8_t opcode){
    return 1;
}

uint8_t  block3_dispatch(dmg* dmg, uint8_t opcode){
    return 1;
}
