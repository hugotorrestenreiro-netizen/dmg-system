#include "c_apu.h"
#include "dmg.h"
#include "memorymap.h"
#include "ppu.h"

uint8_t bus_read(dmg* dmg, uint16_t addr){
    if (addr == 0xFF44) return dmg->PPU.LY;

    if (addr == 0xFF00){
        return joypad_read(dmg);
    }
    if (addr >= 0xFF10 && addr <= 0xFF3F) { //APU ADDR
    return apu_read(addr);
    }
    if (addr == 0xFF04){
        return (dmg->CPU.sys_counter >> 8) & 0xFF;
    }
    return dmg->memory_map[addr>>10][addr & 0x03FF];
}

void bus_write(dmg* dmg, uint16_t addr, uint8_t value){

    //  MBC START
    if(addr < 0x8000){
        if(dmg->MBC.mbc_type == 1){
            if(addr < 0x2000){
                dmg->MBC.ram_enabled = ((value & 0x0F) == 0x0A);
            }
            else if (addr < 0x4000){
                dmg->MBC.rom_bank = value & 0x1F;
                if (dmg->MBC.rom_bank == 0) dmg->MBC.rom_bank = 1;
            }
            else if (addr < 0x6000){
                dmg->MBC.ram_bank = value & 0x03;
            }
            else{ 
                dmg->MBC.banking_mode = value & 0x01;
            }
            update_mbc_banks(dmg);
        }
        else if(dmg->MBC.mbc_type == 3){
            if(addr < 0x2000)       dmg->MBC.ram_enabled = ((value & 0x0F) == 0x0A);
            else if(addr < 0x4000){ dmg->MBC.rom_bank = value & 0x7F;
                            if(dmg->MBC.rom_bank == 0) dmg->MBC.rom_bank = 1; }
            else if(addr < 0x6000)  dmg->MBC.ram_bank = value;
            // 0x6000-0x7FFF
            update_mbc_banks(dmg);
        }
        else if(dmg->MBC.mbc_type == 5){
            if(addr < 0x2000)       dmg->MBC.ram_enabled = ((value & 0x0F) == 0x0A);
            else if(addr < 0x3000)  dmg->MBC.rom_bank = (dmg->MBC.rom_bank & 0x100) | value;
            else if(addr < 0x4000)  dmg->MBC.rom_bank = (dmg->MBC.rom_bank & 0x0FF) | ((value & 1) << 8);
            else if(addr < 0x6000)  dmg->MBC.ram_bank = value & 0x0F;
            update_mbc_banks(dmg);
}
        return;
    }
    //  MBC END

    if (addr >= 0xFF10 && addr <= 0xFF3F) {  //APU ADDR
    apu_write(addr, value);
    return;
    }

    if(addr >= 0xA000 && addr < 0xC000 && !dmg->MBC.ram_enabled) return;

    if(addr == 0xFF41){ //STAT UPDATE
        update_stat_flags(dmg);
    }

    if(addr == 0xFF45){  //LYC UPDATE
        dmg->memory_map[addr>>10][addr & 0x03FF] = value;
        update_stat_flags(dmg);
        return;
    }
    if(addr == 0xFF46){
        uint16_t source = value << 8;
        for(int i = 0; i < 160; i++){
            uint8_t byte = bus_read(dmg, source + i);
            bus_write(dmg, 0xFE00 + i, byte);
        }
    }

    if(addr == 0xFF04){
        dmg->CPU.sys_counter = 0;
        return;
    }

    if (addr == 0xFF00){
        dmg->memory[0xFF00] = value & 0x30;
        return;
    }

    dmg->memory_map[addr>>10][addr & 0x03FF] = value;
    return;
}

void update_flags(dmg* dmg, uint8_t z, uint8_t n, uint8_t h, uint8_t c){
    dmg->CPU.AF = (dmg->CPU.AF & 0xFF00) | (z << 7) | (n << 6) | (h<< 5) | (c << 4);
    return;
}

uint8_t check_cond(dmg* dmg, uint8_t cc){
    uint8_t id_flag = (cc >> 1) & 1 ;     
    uint8_t n_bool = cc & 1 ;
    id_flag = (id_flag * -3) + 7;
    uint8_t flag = (dmg->CPU.AF >> id_flag) & 1;
    return (flag == n_bool);
}

//  JOYPAD

uint8_t joypad_read(dmg* dmg){
    uint8_t joypad_val = dmg->memory[0xFF00];
    uint8_t state_dpad = dmg->joypad_state & 0x0F;
    uint8_t state_buttons = (dmg->joypad_state & 0xF0) >> 4;
    uint8_t low_nibble = 0x0F;
    if(((joypad_val >> 4) & 1) == 0){
        low_nibble = low_nibble & ~(state_dpad);
    }
    if(((joypad_val >> 5) & 1) == 0){
        low_nibble = low_nibble & ~(state_buttons);
    }
    return 0xC0 | (joypad_val & 0x30) | (low_nibble & 0x0F);
}

//  MBC

void update_mbc_banks(dmg* dmg){
    uint8_t actual_rom_bank = dmg->MBC.rom_bank & 0x1F;

    if(dmg->MBC.mbc_type == 5){
        uint32_t off = ((uint32_t)dmg->MBC.rom_bank * 16 * 1024) % dmg->MBC.rom_size;
        for(int i = 0; i < 16; i++) dmg->memory_map[16 + i] = &dmg->MBC.rom_buffer[off + i*1024];
        for(int i = 0; i < 8; i++){
            if(dmg->MBC.ram_enabled) dmg->memory_map[40 + i] = &dmg->MBC.ram_buffer[dmg->MBC.ram_bank * 8 * 1024 + i*1024];
            else                     dmg->memory_map[40 + i] = dmg->MBC.dummy_ram;
        }
        return;
    }

    if(dmg->MBC.mbc_type == 3){
        uint32_t bank = dmg->MBC.rom_bank & 0x7F;
        if(bank == 0) bank = 1;
        uint32_t off = (bank * 16 * 1024) % dmg->MBC.rom_size;
        for(int i = 0; i < 16; i++) dmg->memory_map[16 + i] = &dmg->MBC.rom_buffer[off + i*1024];
        for(int i = 0; i < 8; i++){
            if(dmg->MBC.ram_enabled && dmg->MBC.ram_bank <= 3)
                dmg->memory_map[40 + i] = &dmg->MBC.ram_buffer[dmg->MBC.ram_bank * 8 * 1024 + i*1024];
            else
                dmg->memory_map[40 + i] = dmg->MBC.dummy_ram;
        }
        return;
    }
    
    if(actual_rom_bank == 0) actual_rom_bank = 1;
    if(dmg->MBC.banking_mode == 0){
        actual_rom_bank = actual_rom_bank | (dmg->MBC.ram_bank << 5);
    }
    uint32_t rom_offset = (actual_rom_bank * 16 * 1024) % dmg->MBC.rom_size;

    for(int i = 0; i <16; i++){
        dmg->memory_map[16 + i] = &dmg->MBC.rom_buffer[rom_offset + (i*1024)];
    }

    if(!dmg->MBC.ram_enabled){
        for(int i = 0; i < 8 ; i++){
            dmg->memory_map[40+i] = dmg->MBC.dummy_ram;
        }
    }
    else{
        uint8_t actual_ram_bank;
        if(dmg->MBC.banking_mode == 1) actual_ram_bank = dmg->MBC.ram_bank & 0x03;
        else actual_ram_bank = 0;
        uint32_t ram_offset = (actual_ram_bank * 8 * 1024);
        for (int i = 0; i < 8; i++){
            dmg->memory_map[40+i] = &dmg->MBC.ram_buffer[ram_offset + (i * 1024)];
        }
    }
}