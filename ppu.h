#ifndef PPU_H
#define PPU_H

#include "dmg.h"
#include "memorymap.h"

void ppu_step(dmg* dmg, uint32_t m_cycle);

void ppu_mode_dispatch(dmg* dmg);

void ppu_mode_2_oam_search(dmg* dmg);
void ppu_mode_3_pixel_transfer(dmg* dmg);
void ppu_mode_0_hblank(dmg* dmg);
void ppu_mode_1_vblank(dmg* dmg);

void scanline_rendering(dmg* dmg);
uint8_t get_pixel_color(dmg* dmg, uint8_t x, uint8_t y);
void draw_sprite_scanline(dmg* dmg, dmg_sprite_t sprite);

#endif