#ifndef TEMPSENSOR_H
#define TEMPSENSOR_H

#include <canix/canix.h>
#include <eds-structs.h>
#include <eds_types.h>
#include <inttypes.h>
#include "onewire.h"

typedef struct
{
	/** 
	 * Typ ID; wird vom EDS Block kopiert, um beim Iterieren ueber die
	 * Devices Typen unterscheiden zu koennen
	 */
	uint8_t type;
	/**
	 * die Config Daten aus dem EEPROM 
	 * */
	eds_tempsensor_block_t config;
	/**
	 * Zaehler, um die Zeitabstaende zwischen den Auslese-Zeitpunkten
	 * zu ermoeglichen
	 */
	uint8_t timer_counter;
	/**
	 * ist 1, wenn ein 1wire CONVERT Befehl gesendet wurde. Die naechste
	 * Handlung nach mind. 1 Sekunde ist, den Messwert auszulesen
	 */
	uint8_t convert_sent;
	/**
	 * speichert die zuletzt gemessene Temperatur
	 */
	int16_t temperature;
} device_data_tempsensor;

/**
 * initialisiert den Temperatur-Sensor. Es wird eine (pseudo-)zufaellige
 * Startzeit verwendet, ab der der Sensor in eingestellten, regelmaessigen
 * Abstaenden seine Messwerte umhersendet.
 */
void tempsensor_init(device_data_tempsensor *p, eds_block_p it);

void tempsensor_can_callback(device_data_tempsensor *p, 
		const canix_frame *frame);

void tempsensor_timer_handler(device_data_tempsensor *p);

#endif
