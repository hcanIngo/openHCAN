#ifndef ZEITZONE_H
#define ZEITZONE_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

typedef struct
{
	uint8_t type;
	eds_zeitzone_block_t config;
	eds_block_p it; // EDS Block Pointer, noetig fuer EDS Schreib-Ops
} device_data_zeitzone;

void zeitzone_init(device_data_zeitzone *p, eds_block_p it);

/**
 * returns true wenn die Zeitzone matched, d.h. die aktuelle Zeit in der
 * Zeitzone liegt
 */
uint8_t zeitzone_matches(uint8_t zeitzone_id);

void zeitzone_can_callback(device_data_zeitzone *p,
		const canix_frame *frame);

#endif

