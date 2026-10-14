#ifndef INSTRUCTIONS
#define INSTRUCTIONS

#include "dmg.h"

void ppu_step(dmg* dmg, uint32_t m_cycle);

void ppu_mode_dispatch(dmg* dmg);

void ppu_mode_2_oam_search(dmg* dmg);
void ppu_mode_3_pixel_transfer(dmg* dmg);
void ppu_mode_0_hblank(dmg* dmg);
void ppu_mode_1_vblank(dmg* dmg);

#endif