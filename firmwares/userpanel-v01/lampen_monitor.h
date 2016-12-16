#ifndef LAMPEN_MONITOR_H
#define LAMPEN_MONITOR_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

uint8_t get_count_lampen_states(void);
void monitor_lampen_can_callback(const canix_frame *frame);

#endif
