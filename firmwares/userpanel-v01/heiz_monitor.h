#ifndef HEIZ_MONITOR_H
#define HEIZ_MONITOR_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

void get_count_Tsoll(uint8_t *nUeber0Grad, uint8_t *nUeber15Grad);
void monitor_heiz_can_callback(const canix_frame *frame);

#endif
