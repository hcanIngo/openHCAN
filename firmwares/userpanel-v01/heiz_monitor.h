#ifndef HEIZ_MONITOR_H
#define HEIZ_MONITOR_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

uint8_t get_count_Tsoll_greater(uint8_t TsollSchwelle);
void monitor_heiz_can_callback(const canix_frame *frame);

#endif
