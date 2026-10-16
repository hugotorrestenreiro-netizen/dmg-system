#ifndef C_APU_H
#define C_APU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void apu_init(uint32_t sample_rate, uint32_t device_id);
void apu_write(uint16_t addr, uint8_t val);
uint8_t apu_read(uint16_t addr);
void apu_end_frame(uint32_t t_cycles);

#ifdef __cplusplus
}
#endif

#endif