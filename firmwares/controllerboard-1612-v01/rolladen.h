#ifndef ROLLADEN_H
#define ROLLADEN_H

#include <eds-structs.h>
#include <canix/eds.h>
#include <inttypes.h>

#define MAX_ROLLADEN_GROUPS 4

#define ROLLADEN_READY               0
#define ROLLADEN_COMMAND_STOP        1
#define ROLLADEN_COMMAND_AUF         2
#define ROLLADEN_COMMAND_AB          3
#define ROLLADEN_COMMAND_DIR_CHANGE  4

#define ROLLADEN_STATE_STOP          1
#define ROLLADEN_STATE_AUF_WAIT      2
#define ROLLADEN_STATE_AUF           3
#define ROLLADEN_STATE_STOP_WAIT_1   4 
#define ROLLADEN_STATE_AB_WAIT_1     5
#define ROLLADEN_STATE_AB_WAIT_2     6
#define ROLLADEN_STATE_AB            7
#define ROLLADEN_STATE_STOP_WAIT_2   8
#define ROLLADEN_STATE_STOP_WAIT_3   9
#define ROLLADEN_STATE_DIR_CHANGE_WAIT_1 10
#define ROLLADEN_STATE_DIR_CHANGE_WAIT_2 11
#define ROLLADEN_STATE_DIR_CHANGE_WAIT_3 12

#define ROLLADEN_DIR_AUF             0
#define ROLLADEN_DIR_AB              1


typedef struct
{
	uint8_t type;
	eds_rolladen_block_t config;
	/**
	 * bisher zurueckgelegte Strecke seit dem letzten Kalibrieren
	 */
	uint32_t summe_laufzeit;

	/**
	 * Position des Rolladens in 10tel Sekunden, gemessen vom Stand "unten"
	 */
	int32_t laufzeit;

	/**
	 * Soll-Laufzeit des Rolladens in 10tel Sekunden, gemessen
	 * vom Stand "unten" wenn nichts vorgegeben ist, dann hier -1
	 */
	int32_t soll_laufzeit;

	/**
	 * Command Source Address: Gibt an, von welcher HCAN Adresse der letzte
	 * Befehl kam. Im Falle des Tasters ist es 0.
	 *
	 * Wird verwendet fuer das HCAN_HES_ROLLADEN_POSITION_CHANGED_INFO Frame
	 */
	uint16_t cmdsrc;

	/**
	 * die letzte Richtung (Zustand des dir-Relais (0 = auf, 1 = ab);
	 * wird verwendet, um einen
	 * Richtungswechsel des fahrenden Rolladens vorzunehmen
	 */
	uint8_t lowlevel_last_dir; 

	/**
	 * ROLLADEN_STATE* Konstante
	 */
	uint8_t lowlevel_state;
	uint8_t lowlevel_command;
	uint8_t lowlevel_wait_counter;

	int8_t hilevel_up_event_counter;

} device_data_rolladen;

/**
 * initialisiert den Rolladen
 */
void rolladen_init(device_data_rolladen *p, eds_block_p it);

/**
 * Liefert 1, falls der Rolladen in der gegebenen Gruppe ist
 */
uint8_t rolladen_is_in_group(const device_data_rolladen *p, uint8_t group);

void rolladen_timer_handler(device_data_rolladen *p);

void rolladen_can_callback(device_data_rolladen *p, const canix_frame *frame);


/* Lowlevel Steuerung */

/**
 * Liefert den Stand (bzw. den Sollzustand, sofern dieser noch nicht
 * aufgrund einer Wartezeit erreicht ist
 */
uint8_t rolladen_lowlevel_get_state(const device_data_rolladen *p);

/**
 * Liefert die aktuelle Position. Der Wertebereich geht von 0 = unten bis
 * 100 = oben.
 */
uint8_t rolladen_lowlevel_get_position(const device_data_rolladen *p);

/**
 * Fuer den Service-Fall: Definiert, d.h. setzt den internen Stand des
 * Rolladens
 */
void rolladen_lowlevel_define_position(device_data_rolladen *p, uint8_t pos);

/**
 * der Timer Handler der Lowlevel Steuerung. Er wird vom rolladen_timer_handler
 * aufgerufen und erledigt die Lowlevel Timer Dinge.
 */
void rolladen_lowlevel_timer_handler(device_data_rolladen *p);

/**
 * begrenzt die Position auf den Bereich [0..Strecke]
 */
void rolladen_position_align(device_data_rolladen *p);

/**
 * Liefert die Minimal-Postion (normalerweise -50% der Strecke). Die
 * Minimal-Postion ist negativ, damit der Rolladen auf jeden Fall ganz
 * unten ankommt und der Endschalter ausloest.
 */
int32_t rolladen_min_laufzeit(device_data_rolladen *p);

/**
 * Liefert die Maximal-Postion (normalerweise 150% der Strecke). Die
 * Maximal-Position ist mehr als 100%, damit der Rolladen auf 
 * jeden Fall ganz oben ankommt und der Endschalter ausloest.
 */
int32_t rolladen_max_laufzeit(device_data_rolladen *p);

/** 
 * Setzt die Soll-Postion anhand der Prozent-Angabe
 */
void rolladen_set_soll_laufzeit(device_data_rolladen *p, uint8_t percent);

#endif
