#ifndef REEDKONTAKT_H
#define REEDKONTAKT_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define REEDKONTAKT_MODUS_AUS        0
#define REEDKONTAKT_MODUS_AUTOMATIK  1

typedef struct
{
	uint8_t type;
	eds_reedkontakt_block_t config;
	uint8_t timer_counter;
	/**
	 * speichert den letzten Zustand des Reedkontakts:
	 * 0 = geschlossen, 1 = offen
	 */
	uint8_t last_state; 
} device_data_reedkontakt;
 
void reedkontakt_init(device_data_reedkontakt *p, eds_block_p it);
void reedkontakt_timer_handler(device_data_reedkontakt *p);
void reedkontakt_can_callback(device_data_reedkontakt *p, 
		const canix_frame *frame);

#endif
