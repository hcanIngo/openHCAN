#ifndef ROLLADEN_H
#define ROLLADEN_H

#include <eds-structs.h>
#include <canix/eds.h>
#include <inttypes.h>

/* Relais-Schaltung fuer den Rolladenantrieb:

                               NC
                               o-----> Rolladen hoch
             \                /
	L  ----o  o--------------o
          NO  COM          COM
	                           o-----> Rolladen runter
                               NO

      Power-Relais      Direction-Relais


Steuerung:
----------

  SollPos          +----------+
  -------------->  |          |  ------> Dir-Relais
                   |          |
  IstPos           |          |
  <--------------  |          |  ------> Power-Relais
                   |          |
                   +----------+

Position:      0 = unten, 100 = oben

siehe: https://github.com/hcanIngo/openHCAN/wiki/Rolladensteuerbefehlsreaktionstabelle
*/


#define MAX_ROLLADEN_GROUPS 4

#define FEATURE_KALIBRIERUNG_OBEN   0 // Bit0 (das LSB)

#define ROLLADEN_DIR_AUF 0
#define ROLLADEN_DIR_AB  1


typedef struct
{
	uint8_t type;
	eds_rolladen_block_t config;

	uint8_t kalibrieren;     // Kalibrierung wird durchgefuehrt = 1, sonst = 0
	uint32_t summe_laufzeit; // bisher zurueckgelegte Strecke seit dem letzten Kalibrieren
	int32_t laufzeit;        // Position des Rolladens in 10tel Sekunden, gemessen vom Stand "unten"
	int32_t soll_laufzeit;   // Soll-Laufzeit des Rolladens in 10tel Sekunden, gemessen vom Stand "unten"
	int32_t stoppuhr;

	uint8_t dir;        // Ist-Zustand des dir-Relais
	uint8_t soll_dir;   // Soll-Zustand des dir-Relais   (1 = zu, 0 = auf)
	uint8_t exe_dir;    // execute: Dir-Relais ansteuern
	uint8_t last_dir;   // Letzte Richtung fuer den 1-Taster-Betrieb

	uint8_t soll_power; // Soll-Zustand des power-Relais (0 = Rolladen nicht in Bewegung, 1 = Rolladen faehrt oder soll fahren)
	uint8_t power;      // Ist-Zustand des power-Relais
	uint8_t exe_power;  // execute: Power-Relais ansteuern

	uint8_t blockingTimer; // Wartezeit, bevor das Power-/ bzw. das Dir-Relais angesprochen werden darf
	int8_t change_dir_counter; // 1-Taster-Betrieb: Langes Stopp-Druecken fuehrt zu anschließendem Richtungswechsel
	int8_t long_pressed_counter; // 2-Taster-Betrieb: Langes Fahr-ZU-Druecken fuehrt zur Kalibrierung in Untenlage
} device_data_rolladen;


void rolladen_init(device_data_rolladen *p, eds_block_p it);
inline void rolladen_timer_handler(device_data_rolladen *p, uint8_t zyklus);
void rolladen_can_callback(device_data_rolladen *p, const canix_frame *frame);

#endif
