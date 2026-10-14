#include "dmg.h"

// INITIATE
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
    dmg->memory[0xFF00] = 0xC0;

    dmg->CPU.AF = 0x01B0;  dmg->CPU.BC = 0x0013;
    dmg->CPU.DE = 0x00D8;  dmg->CPU.HL = 0x014D;
    dmg->memory[0xFF40] = 0x91; 
    dmg->memory[0xFF47] = 0xFC;

    return dmg;
}

uint16_t* reg16stk_index(dmg* dmg, uint8_t input){
    uint16_t* reg16stk[4] = {
    &dmg->CPU.BC, 
    &dmg->CPU.DE,
    &dmg->CPU.HL,
    &dmg->CPU.AF
    };
    return reg16stk[input];
}

//  REG INDEX HELPER

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

//  ROM LOADING

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

//  JOYPAD

void key_up(dmg* dmg, dmg_key_t key){
    dmg->joypad_state = dmg->joypad_state & ~key;
    return;
}

void key_down(dmg* dmg, dmg_key_t key){
    dmg->joypad_state = dmg->joypad_state | key;
    uint8_t joypad_val = dmg->memory[0xFF00];
    if((key < 4 && ((joypad_val >> 4) & 1) == 0)){
        dmg->memory[0xFF0F] |=  (1<<4);
    }
    if((key >= 4 &&((joypad_val >> 5) & 1) == 0)){
        dmg->memory[0xFF0F] |=  (1<<4);
    }
    return;
}

//      FETCH DECODE EXECUTE

void cpu_step(dmg* dmg){
    uint8_t cycle;
    uint8_t pending = (dmg->memory[0xFF0F] & dmg->memory[0xFFFF] & 0x1F);
        //  IME
    if((dmg->CPU.ime == 1) && pending){
        dmg->CPU.ime = 0;
        dmg->CPU.halt = 0;
        dmg->CPU.SP--;
        bus_write(dmg, dmg->CPU.SP, (dmg->CPU.PC >> 8) & 0xFF);
        dmg->CPU.SP--;
        bus_write(dmg, dmg->CPU.SP, dmg->CPU.PC & 0xFF);
        for (int i = 0; i < 5; i++){
            if(pending & (1 << i)){
                dmg->memory[0xFF0F] &= ~(1 << i);
                dmg->CPU.PC = 0x0040 + (i*8);
                break;
            }
        }
        dmg->CPU.cycle += 5;
    }
    //  HALT
    else if (dmg->CPU.halt == 1){
        dmg->CPU.cycle += 1;
        uint8_t halt_stop = (dmg->memory[0xFF0F] & dmg->memory[0xFFFF] & 0x1F);
        dmg->CPU.halt = !halt_stop;
    }
    //  NORMAL CPU
    else if(dmg->CPU.halt == 0){
        if(dmg->CPU.ime_scheduled){
            dmg->CPU.ime = 1;
            dmg->CPU.ime_scheduled=0;
        }
        uint8_t opcode = bus_read(dmg, dmg->CPU.PC);
        cycle = instruction_table(dmg, opcode);
        dmg->CPU.cycle += cycle;
    }
        
}

//  TIMERS

void timer_step(dmg* dmg, uint32_t m_cycle){
    uint32_t t_cycle = 4 * m_cycle;
    const static uint8_t tac_bit_target[4] = { 9, 3, 5, 7 };
    
    uint8_t tac_reg = bus_read(dmg, 0xFF07);
    uint8_t tac_target = (tac_reg & 0x03);
    uint8_t tac_enable = ((tac_reg & 4) >>2) ;

    uint8_t bit_pos = tac_bit_target[tac_target];
    for(int i = 0; i< t_cycle; i++){
        uint8_t tac_old = tac_enable * ((dmg->CPU.sys_counter >> bit_pos) & 1);
        dmg->CPU.sys_counter ++;
        uint8_t tac_new = tac_enable * ((dmg->CPU.sys_counter >> bit_pos) & 1);
        
        if((tac_old == 1) && (tac_new == 0)){
            uint8_t tima = bus_read(dmg,0xFF05) + 1;
            if(tima == 0){
                tima = bus_read(dmg, 0xFF06);
                bus_write(dmg, 0xFF0F, bus_read(dmg, 0xFF0F) | (1<<2));
            }
            bus_write(dmg, 0xFF05, tima);
        }
    }
}