#include "ppu.h"

void ppu_step(dmg* dmg, uint32_t m_cycle){
    uint32_t t_cycle = 4*m_cycle;
    for (int i = 0; i < t_cycle; i++){
        ppu_mode_dispatch(dmg);
        dmg->PPU.ppu_cycle++;
    }
    return;
}

void ppu_mode_dispatch(dmg* dmg){
    switch (dmg->PPU.mode)
    {
    case 2:
        ppu_mode_2_oam_search(dmg);
        break;
    case 3:
        ppu_mode_3_pixel_transfer(dmg);
        break;
    case 0:
        ppu_mode_0_hblank(dmg);
        break;
    case 1:
        ppu_mode_1_vblank(dmg);
        break;
    default:
        break;
    }
}

void ppu_mode_2_oam_search(dmg* dmg){
    if(dmg->PPU.ppu_cycle < 80){
        return;
    }

    dmg->PPU.sprite_count = 0;
    uint8_t h;
    uint8_t reg_lcdc = bus_read(dmg, 0xFF40);

    if((reg_lcdc & 0x04) != 0) h = 16;
    else h = 8;

    for(int i = 0xFE00; i <= 0xFE9F; i+=4){
        int16_t y = (int16_t)bus_read(dmg, i) - 16;
            if (dmg->PPU.LY >= y && dmg->PPU.LY < (y + h)){
                dmg->PPU.selected_sprite[dmg->PPU.sprite_count++] = i;

                if(dmg->PPU.sprite_count == 10){
                    break;
                }
            }
    }
    dmg->PPU.mode = 3;
    return;
}

void ppu_mode_3_pixel_transfer(dmg* dmg){

}

void ppu_mode_0_hblank(dmg* dmg){

}

void ppu_mode_1_vblank(dmg* dmg){
    
}
