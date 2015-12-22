#ifndef TASTER_H
#define TASTER_H

#include <canix/canix.h>
#include <inttypes.h>
#include <eds-structs.h>

#define FEATURE_TASTER_ENTPRELL_1S   0 // Bit0 (das LSB)
#define FEATURE_TASTER_ROLLADEN_AUF  1 // Bit1
#define FEATURE_TASTER_ROLLADEN_ZU   2 // Bit2
#define FEATURE_TASTER_POWERPORT_AN  3 // Bit3
#define FEATURE_TASTER_POWERPORT_AUS 4 // Bit4

typedef struct
{
	uint8_t type;
	eds_taster_block_t config;
	uint8_t pressed; // fuer Entprellung
} device_data_taster;

void taster_timer_handler(device_data_taster *p);

#endif

