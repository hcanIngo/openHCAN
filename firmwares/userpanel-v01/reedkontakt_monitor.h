#ifndef REEDKONTAKT_MONITOR_H
#define REEDKONTAKT_MONITOR_H

#include <inttypes.h>
#include <canix/canix.h>
#include <eds-structs.h>

uint8_t get_count_reedkontakt_states(void);
void monitor_reedkontakt_can_callback(const canix_frame *frame);


#endif
