#include "ppu.h"

void ppu_step(dmg* dmg, uint32_t m_cycle){
    uint8_t lcdc = bus_read(dmg, 0xFF40);
    if ((lcdc & 0x80) == 0) {
        dmg->PPU.ppu_cycle = 0;
        dmg->PPU.LY = 0;
        dmg->PPU.mode = 0;
        return;
    }
    uint32_t t_cycle = 4*m_cycle;
    for (int i = 0; i < t_cycle; i++){
        ppu_mode_dispatch(dmg);
        dmg->PPU.ppu_cycle++;
    }
    return;
}

void ppu_mode_dispatch(dmg* dmg){
    uint8_t stat = bus_read(dmg, 0xFF41);
    bus_write(dmg, 0xFF41, (stat & 0xFC) | (dmg->PPU.mode & 0x03));
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

//      MODE 2

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
                dmg_sprite_t sprite;
                sprite.y = y;
                sprite.x = (int16_t) bus_read(dmg, i + 1) - 8;
                sprite.tile = bus_read(dmg, i + 2);
                sprite.flags = bus_read(dmg, i + 3);
                dmg->PPU.selected_sprite[dmg->PPU.sprite_count++] = sprite;

                if(dmg->PPU.sprite_count == 10){
                    break;
                }
            }
    }
    dmg->PPU.mode = 3;
    return;
}

//      MODE 3

void ppu_mode_3_pixel_transfer(dmg* dmg){
    if(dmg->PPU.ppu_cycle >= 252){
        scanline_rendering(dmg);
        dmg->PPU.mode = 0;
    }
}

void scanline_rendering(dmg* dmg) {
    uint8_t lcdc = bus_read(dmg, 0xFF40);

    for (int x = 0; x < 160; x++) {
        uint8_t color = get_pixel_color(dmg, x, dmg->PPU.LY);
        dmg->PPU.display[dmg->PPU.LY * 160 + x] = color;
    }

    if ((lcdc & 0x02) != 0) {
        for (int i = dmg->PPU.sprite_count - 1; i >= 0; i--) {
            draw_sprite_scanline(dmg, dmg->PPU.selected_sprite[i]);
        }
    }
}

uint8_t get_pixel_color(dmg* dmg, uint8_t x, uint8_t y) {
    uint8_t lcdc = bus_read(dmg, 0xFF40);
    uint8_t wy = bus_read(dmg, 0xFF4A);
    uint8_t wx = bus_read(dmg, 0xFF4B);

    uint8_t fetch_x;
    uint8_t fetch_y;
    uint16_t map_base;

    if (((lcdc & 0x20) != 0) && (y >= wy) && ((x + 7) >= wx)) {
        if ((lcdc & 0x40) != 0) {
            map_base = 0x9C00;
        } else {
            map_base = 0x9800;
        }

        fetch_x = x - (wx - 7);
        fetch_y = y - wy;
    } else {
        if ((lcdc & 0x08) != 0) {
            map_base = 0x9C00;
        } else {
            map_base = 0x9800;
        }

        fetch_x = x + bus_read(dmg, 0xFF43);
        fetch_y = y + bus_read(dmg, 0xFF42);
    }

    uint16_t map_addr = map_base + (fetch_y / 8) * 32 + (fetch_x / 8);
    uint8_t tile_num = bus_read(dmg, map_addr);

    uint16_t tile_addr;
    if ((lcdc & 0x10) != 0) {
        tile_addr = 0x8000 + (tile_num * 16) + ((fetch_y % 8) * 2);
    } else {
        int8_t signed_tile = (int8_t)tile_num;
        tile_addr = 0x9000 + (signed_tile * 16) + ((fetch_y % 8) * 2);
    }

    uint8_t byte1 = bus_read(dmg, tile_addr);
    uint8_t byte2 = bus_read(dmg, tile_addr + 1);

    uint8_t bit = 7 - (fetch_x % 8);
    uint8_t color_id = (((byte2 >> bit) & 1) << 1) | ((byte1 >> bit) & 1);
    uint8_t bgp = bus_read(dmg, 0xFF47);

    return (bgp >> (color_id * 2)) & 0x03;
}

void draw_sprite_scanline(dmg* dmg, dmg_sprite_t sprite){
    uint8_t line_sprite = dmg->PPU.LY - sprite.y;

    uint8_t h;
    uint8_t reg_lcdc = bus_read(dmg, 0xFF40);

    if((reg_lcdc & 0x04) != 0) h = 16;
    else h = 8;

    if((sprite.flags & 0x40) != 0) line_sprite = (h - 1) - line_sprite;
    if(h == 16) sprite.tile &= 0xFE;

    uint16_t tile_addr = 0x8000 + (sprite.tile * 16) + (line_sprite * 2);
    uint8_t byte1 = bus_read(dmg, tile_addr);
    uint8_t byte2 = bus_read(dmg, tile_addr + 1);

    uint16_t pal_reg;
    if((sprite.flags & 0x10) != 0) pal_reg = 0xFF49;
    else pal_reg = 0xFF48;
    uint16_t palette = bus_read(dmg, pal_reg);

    for(int px = 0; px < 8; px++){
        int screen_x = sprite.x + px;
        if(screen_x < 0 || screen_x >= 160) continue;
        uint8_t bit;
        if ((sprite.flags & 0x20) != 0) bit = px;
        else bit = 7 - px;
        uint8_t color_id = (((byte2 >> bit) & 1) << 1) | ((byte1 >> bit)& 1);
        if(color_id == 0) continue;
        uint8_t color = (palette >> (color_id *2)) & 0x03;
        dmg->PPU.display[dmg->PPU.LY * 160 + screen_x] = color;
    }
}

//      MODE 0

void ppu_mode_0_hblank(dmg* dmg){
    if(dmg->PPU.ppu_cycle >= 456){
        dmg->PPU.ppu_cycle -=456;
        dmg->PPU.LY++;
        if(dmg->PPU.LY >= 144){
            dmg->PPU.mode = 1;
            uint8_t if_reg = bus_read(dmg, 0xFF0F);
            bus_write(dmg, 0xFF0F, if_reg | 0x01);
        }
        else{
            dmg->PPU.mode = 2; 
        }
        return;
    }
    return;
}
    

//      MODE 1

void ppu_mode_1_vblank(dmg* dmg){
    if (dmg->PPU.ppu_cycle >= 456){
        dmg->PPU.ppu_cycle -=456;
        dmg->PPU.LY++;
        if (dmg->PPU.LY > 153){
            dmg->PPU.LY = 0;
            dmg->PPU.mode = 2;
        }
    }
}
