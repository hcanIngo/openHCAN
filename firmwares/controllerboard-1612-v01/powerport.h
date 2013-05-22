#ifndef POWERPORT_H
#define POWERPORT_H

#include <eds-structs.h>
#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define MAX_POWERPORT_GROUPS 4

#define FALLENDE_FLANKE 	0 // powerport 1->0
#define STEIGENDE_FLANKE 	1 // powerport 0->1

#define POWERPORT_FEATURE_BISTABLE_RELAIS   0 // Bit0 (the LSB)
/**
 * powerport-Timertrigger ist ein HES-Telegramm z.B. HCAN_HES_TASTER_DOWN:
 * ------------------------------------------------------------------------
 * Timer-Feature-Name		 	VERZOEGERT_EIN		AUTO_AUS		 NACHLAUF
 * powerport-State vorher	 	powerport aus		powerport aus	 powerport ein
 * Reaktion auf Timertrigger 	keine				powerport ein	 keine
 * Reaktion auf Timerablauf	 	powerport ein		powerport aus	 powerport aus	 */
#define POWERPORT_FEATURE_VERZOEGERT_EIN		1 // Bit1 (z.B. Badzimmerluefter)
#define POWERPORT_FEATURE_AUTO_AUS				2 // Bit2 (z.B. Speisekammerluefter, Haustuerklingel, Treppenhausbeleuchtung)
#define POWERPORT_FEATURE_NACHLAUF	 			3 // Bit3 (z.B. Beamersteckdose)
/* Die folgenden "temporaere" Features sind in weck_page zu setzen.
   Hier sind sie aufgefuehrt da sie auch im Powerport
   die entsprechenden Bits belegen: */
#define POWERPORT_FEATURE_WECK_VERZOEGERT_EIN	4 // Bit4 (z.B. Kurzzeitwecker: verzoegert ein)
#define POWERPORT_FEATURE_WECK_AUTO_AUS			5 // Bit5 (z.B. Backofentimer: sofort ein, verzoegert aus)


typedef struct
{
	uint8_t type;
	eds_powerport_block_t config;
	uint16_t countDownTimer; // ungleich 0 -> Timer aktiv
	uint8_t bistableRelaisState; // sichert den aktuellen Schaltzustand eines bistabilen Relais
} device_data_powerport;

void powerport_init(device_data_powerport *p);

/**
 * Der powerport_timer_handler wird einmal pro Sekunde aufgerufen
 */
void powerport_timer_handler(device_data_powerport *p);

/**
 * Der adressierte Powerport wechselt den Zustand
 */
void powerport_toggle(device_data_powerport *p);

/**
 * Der Zustand des Powerports wird gesetzt
 * @param p Powerport
 * @param state 0 = aus, 1 = ein
 */
void powerport_set(device_data_powerport *p, uint8_t state);

/**
 * Liefert den aktuellen Zustand des Powerports
 */
uint8_t powerport_get(const device_data_powerport *p);

/**
 * Liefert != 0, falls der Powerport in der genannten Gruppe ist
 */
uint8_t powerport_is_in_group(const device_data_powerport *p, uint8_t group);

void powerport_can_callback(device_data_powerport *p, const canix_frame *frame);

/**
 * Sendet den aktuellen Status als Info Message mit der pirmaeren
 * Gruppen-ID (gruppe0)
 */
void powerport_send_state_info(device_data_powerport *p);


void powerport_setup_timer(device_data_powerport *p);
void set_powerport_or_timer(device_data_powerport *p, uint8_t Flanke);
void powerport_on(device_data_powerport *p);
void powerport_off(device_data_powerport *p);


#endif
