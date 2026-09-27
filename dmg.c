#include "dmg.h"

uint8_t* reg8bit_map[8];

dmg* initiate_dmg(){
    dmg* dmg;
    dmg = calloc(1,sizeof(*dmg));
    if(dmg == NULL)return NULL;
    
    reg8bit_map[0] = ((uint8_t*) &dmg->CPU.BC) + 1;
    reg8bit_map[1] = ((uint8_t*) &dmg->CPU.BC) + 0;
    reg8bit_map[2] = ((uint8_t*) &dmg->CPU.DE) + 1;
    reg8bit_map[3] = ((uint8_t*) &dmg->CPU.DE) + 0;
    reg8bit_map[4] = ((uint8_t*) &dmg->CPU.HL) + 1;
    reg8bit_map[5] = ((uint8_t*) &dmg->CPU.HL) + 0;
    reg8bit_map[6] = NULL;
    reg8bit_map[7] = ((uint8_t*) &dmg->CPU.AF) + 1;

    return dmg;
}