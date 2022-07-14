#ifndef __MORPHEUS_STATES__
#define __MORPHEUS_STATES__

#include <stdint.h>

void morpheus_state_init();
void morpheus_sleep_pin(uint16_t pre_delay, bool pin_state);
void morpheus_sleep_time(uint16_t pre_delay, uint32_t sleep_time);

#endif
