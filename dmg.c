#include "dmg.h"


dmg* initiate_dmg(){
    dmg* dmg;
    dmg = calloc(1,sizeof(*dmg));
    if(dmg == NULL)return NULL;
    
    dmg->CPU.reg8bit_map[0] = ((uint8_t*) &dmg->CPU.BC) + 1;
    dmg->CPU.reg8bit_map[1] = ((uint8_t*) &dmg->CPU.BC) + 0;
    dmg->CPU.reg8bit_map[2] = ((uint8_t*) &dmg->CPU.DE) + 1;
    dmg->CPU.reg8bit_map[3] = ((uint8_t*) &dmg->CPU.DE) + 0;    //mapping register (à changer (pipeline flush car switch))
    dmg->CPU.reg8bit_map[4] = ((uint8_t*) &dmg->CPU.HL) + 1;
    dmg->CPU.reg8bit_map[5] = ((uint8_t*) &dmg->CPU.HL) + 0;
    dmg->CPU.reg8bit_map[6] = NULL ;
    dmg->CPU.reg8bit_map[7] = ((uint8_t*) &dmg->CPU.AF) + 1;

    return dmg;
}

uint8_t* reg8bit_index(dmg* dmg,uint8_t input){
    if(input == 6){
        return &dmg->memory[dmg->CPU.HL];
    }
    return (dmg->CPU.reg8bit_map[input]);
}