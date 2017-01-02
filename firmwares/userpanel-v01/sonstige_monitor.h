#ifndef SONSTIGE_MONITOR_H
#define SONSTIGE_MONITOR_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

uint8_t get_count_sonstige_states(uint8_t countIgnore);
void monitor_sonstige_can_callback(const canix_frame *frame);

#endif
