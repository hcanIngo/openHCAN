#ifndef MUTE_MONITOR_H
#define MUTE_MONITOR_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

uint8_t get_count_mute_states(void);
void monitor_mute_can_callback(const canix_frame *frame);

#endif
