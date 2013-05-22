#ifndef TASTDIMMER_H
#define TASTDIMMER_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define MAX_TASTDIMMER_GROUPS 8

typedef struct
{
	uint8_t type;
	eds_tastdimmer_block_t config;
} device_data_tastdimmer;

/**
 * Liefert != 0, falls der Powerport in der genannten Gruppe ist
 */
uint8_t tastdimmer_is_in_group(const device_data_tastdimmer *p, uint8_t group);

void tastdimmer_can_callback(device_data_tastdimmer *p, const canix_frame *frame);

#endif
