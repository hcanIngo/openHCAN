#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <canix/canix.h>
#include <eds-structs.h>
#include <inttypes.h>

typedef struct
{
	uint8_t type;

	eds_timeservice_block_t config;

	/**
	 * besagt, vor wievielen Sekunden das letzte Time Frame
	 * empfangen wurde
	 */
	uint8_t last_time_frame_received;
	/**
	 * besagt, vor wievielen Sekunden das letzte Time Frame
	 * gesendet wurde
	 */
	uint8_t last_time_frame_sent;
} device_data_timeservice;

void timeservice_can_callback(const canix_frame *frame);
void timeservice_timer_handler(device_data_timeservice *p);

#endif

