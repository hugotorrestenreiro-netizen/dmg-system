#include "dmg.h"


dmg* initiate_dmg(){
    dmg* dmg;
    dmg = calloc(1,sizeof(*dmg));
    if(dmg == NULL)return NULL;
    
    for(int i= 0; i < 64 ; i++){
        dmg->memory_map[i] = &dmg->memory[1024 * i];
    }

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

uint16_t reg16bit_mem_index(dmg* dmg,uint8_t input){
    uint16_t* reg16map[4]= {&dmg->CPU.BC, &dmg->CPU.DE, &dmg->CPU.HL, &dmg->CPU.HL};
    static const int8_t mov[4]= {0,0,1,-1};
    uint16_t addr = *reg16map[input];
    *reg16map[input] += mov[input];
    return addr;
}

uint16_t* reg16bit_index(dmg* dmg,uint8_t input){
    return (dmg->CPU.reg16bit_map[input]);
}


uint8_t get_reg8bit_index(dmg* dmg,uint8_t input){
    if(input == 6){ 
        return bus_read(dmg, dmg->CPU.HL);
    }
    return *(dmg->CPU.reg8bit_map[input]);
}

void set_reg8bit_index(dmg* dmg, uint8_t input, uint8_t value){
    if(input == 6){ 
        bus_write(dmg, dmg->CPU.HL, value);
        return;
    }
    *dmg->CPU.reg8bit_map[input] = value;
    return;
}

uint8_t* load_rom(dmg* dmg, char* file_loc){
    if(file_loc == NULL){
        printf("file location error");
        return NULL;
    } 
    char* extension;
    extension = strrchr(file_loc , '.');
    if(extension && strcmp(extension, ".gb") == 0){
        FILE* file = fopen(file_loc, "rb");
        if(file == NULL){
            printf("fopen error");
            return NULL;
        }
        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        rewind(file);
        uint8_t* rom_buffer = malloc(size);
        if(rom_buffer == NULL){
            fclose(file); 
            printf("rom_buffer malloc error");
            return NULL;
        }
        fread(rom_buffer, 1, size, file);
        fclose(file);
       for (int i = 0; i < 32; i++) {
        if (i * 1024 < size) {
            dmg->memory_map[i] = &rom_buffer[i * 1024];
        }
        else {
            dmg->memory_map[i] = &dmg->memory[i * 1024];
            }
        }
        return rom_buffer;
    }
    return NULL;
}

// void update_flags_add8()