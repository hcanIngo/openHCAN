#ifndef ZEITSCHALTUHR_H
#define ZEITSCHALTUHR_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define ZEITSCHALTUHR_UPDATE_INTERVAL 173

typedef struct
{
	uint8_t type;
	eds_zeitschaltuhr_block_t config;

	/** hier wird der Zustand der Power-Gruppe gespeichert */
	uint8_t state;

	/** 
	 * In regelmaessigen Abstaenden wird der Zustand wiederholt an die
	 * Power Gruppe gesendet, so dass verlorene Frames oder rebootete 
	 * Geraete keine Probleme machen.
	 */
	uint16_t update_counter;
	uint8_t automatikEin; // Automatik aktiviert oder nicht
} device_data_zeitschaltuhr;

void zeitschaltuhr_init(device_data_zeitschaltuhr *p, eds_block_p it);
void zeitschaltuhr_timer_handler(device_data_zeitschaltuhr *p);
void zeitschaltuhr_can_callback(device_data_zeitschaltuhr *p, const canix_frame *frame);

#endif
