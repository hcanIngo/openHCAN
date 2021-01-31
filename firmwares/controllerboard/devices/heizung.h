#ifndef HEIZUNG_H
#define HEIZUNG_H

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define HEIZUNG_MODE_OFF          0
#define HEIZUNG_MODE_MANUAL       1
#define HEIZUNG_MODE_THERMOSTAT   2
#define HEIZUNG_MODE_AUTOMATIK    3

//PID
#define SOLLISTWERTE			3
#define ABSTANDPIDMESSUNGEN		30		// 60 sekunden
#define PWMCHANGEFREQENCY		300		// 300 Sekunden

/**
 * Falls 2 HK parallel angesteuert werden, kann durch
 * Setzen dieses Feature-Bits erwirkt werden, dass bei
 * Waermebedarf nicht 100, sondern 200% angefordert werden.
 */
#define HEIZUNG_FEATURE_DOPPEL_WAERMEBEDARF   0
#define HEIZUNG_FEATURE_VENTIL_IMMER_PFLEGEN  1

typedef struct
{
	uint8_t id;
	int16_t temp;
} solltemp_line_t;

/*
 */

typedef struct
{
	uint8_t type;
	eds_heizung_block_t config;
	eds_block_p it; // EDS Block Pointer, noetig fuer EDS Schreib-Ops
	uint8_t mode;             // see HEIZUNG_MODE_* above
	uint8_t manual_rate;      // if in manual mode, this is the heating rate
	uint8_t timer_counter;     // just update all 30 Seconds !
	uint16_t pwm_counter;
	uint16_t pwm_width;
	uint16_t pwm_end;
	uint8_t waermebedarf_counter;
	uint16_t pwm_change_counter;
	uint8_t received_interval_counter;
	int16_t measure_value;
	int16_t destination_value;
	uint16_t duration_counter;   // remaining time in Manual/Thermostat-Mode
	int16_t thermostat_temp;    // Soll-Temperatur im Thermostat-Modus
	int16_t last_soll_temp;    // Soll-Temperatur im Thermostat-Modus
	uint8_t reed_heiz_stop_counter;
	/**
	 * Wenn die Heizung zu einer bestimmten Tageszeit (abhaengig von der
	 * ID, damit nicht alle gleichzeitig) im Modus HEIZUNG_MODE_OFF ist,
	 * wird der ventilpflege_counter auf 600 (Sekunden = 10min) gesetzt 
	 * und das Ventil geoeffnet. Ist der ventilpflege_counter auf 0, so
	 * wird das Ventil wieder geschlossen. In der Zeit wird keine
	 * Waermeanforderung gesendet.
	 */
	uint16_t ventilpflege_counter;
	// PID
	uint8_t pfaktor;	// als 1/100
	uint8_t ifaktor;	// als 1/100
	uint8_t dfaktor;	// als 1/100
	uint8_t pidcounter;			// sekunden seit der letzen messung/aufzeichnung
	int32_t pidvalue;
	//int16_t sollist0;	// letzter Soll - ist als 1/16 Grad
	//int16_t sollist1;	// vorletzter Soll - ist als 1/16 Grad
	//int16_t sollist2;	// vorvorletzter Soll - ist als 1/16 Grad
	int16_t sollist[SOLLISTWERTE]; // Soll - ist als 1/16 Grad
	uint8_t sollistIndex;
} device_data_heizung;

void heizung_init(device_data_heizung *p, eds_block_p it);
void heizung_can_callback(device_data_heizung *p, const canix_frame *frame);
extern void heizung_timer_handler(device_data_heizung *p, uint8_t zyklus);

#endif
