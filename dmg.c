#include "dmg.h"


dmg* initiate_dmg(){
    dmg* dmg;
    dmg = calloc(1,sizeof(*dmg));
    if(dmg == NULL)return NULL;
    
    dmg->CPU.reg8bit_map[0] = ((uint8_t*) &dmg->CPU.BC) + 1;
    dmg->CPU.reg8bit_map[1] = ((uint8_t*) &dmg->CPU.BC) + 0;
    dmg->CPU.reg8bit_map[2] = ((uint8_t*) &dmg->CPU.DE) + 1;
    dmg->CPU.reg8bit_map[3] = ((uint8_t*) &dmg->CPU.DE) + 0;    //mapping register (faire attention et implémenter la "bascule" de systeme BIG ENDIAN et LITTLE ENDIANT)
    dmg->CPU.reg8bit_map[4] = ((uint8_t*) &dmg->CPU.HL) + 1;
    dmg->CPU.reg8bit_map[5] = ((uint8_t*) &dmg->CPU.HL) + 0;
    dmg->CPU.reg8bit_map[6] = NULL ;
    dmg->CPU.reg8bit_map[7] = ((uint8_t*) &dmg->CPU.AF) + 1;

    dmg->CPU.reg16bit_map[0] = &dmg->CPU.BC;
    dmg->CPU.reg16bit_map[1] = &dmg->CPU.DE;
    dmg->CPU.reg16bit_map[2] = &dmg->CPU.HL;
    dmg->CPU.reg16bit_map[3] = &dmg->CPU.SP;

    dmg->CPU.SP = 0xFFFE;
    dmg->CPU.PC = 0x0100;

    return dmg;
}

uint8_t* reg8bit_index(dmg* dmg,uint8_t input){
    if(input == 6){                //temp  value will nbe initialized in main while loop
        return &dmg->memory[dmg->CPU.HL];
    }
    return (dmg->CPU.reg8bit_map[input]); //WARNING !!!! (memo: Quand tu feras la boucle main met juste avant execute la ligne "dmg->CPU.reg8bit_map[6]=&dmg->memory[dmg->CPU.HL]")
}