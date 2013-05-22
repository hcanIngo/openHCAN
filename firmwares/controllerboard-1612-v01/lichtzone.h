#ifndef LICHTZONE_H
#define LICHTZONE_H

#include <canix/canix.h>
#include <eds-structs.h>
#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

#define LICHTZONE_STATE_OFF 0
#define LICHTZONE_STATE_ON 1
#define LICHTZONE_STATE_ON_EXT 2

#define LICHTZONE_EXT_TIMERINIT 4*60*60 // 4 Stunden
#define LICHTZONE_EXT_SEC_COUNTER_MAX 3

/**
 * Reaktion auf Toggle-Taster im normaler Timer Modus:
 *
 * - wenn DOWN: es wird eingeschaltet und der timer auf den startwert
 *   gestetzt
 * - wenn DOWN waehrend timer, dann Timer wieder auf startwert setzen
 * - wenn der timer abgelaufen ist, wird ausgeschaltet
 *
 * Reaktion auf Toggle-Taster im Extended On Modus:
 *
 * - wenn 3x DOWN innerhalb von 3 Sekunden, dann einschalten und den
 *   startwert auf LICHTZONE_EXT_TIMERINIT (4 Stunden) setzen 
 * - wenn DOWN, dann ausscchalten
 * - wenn timer abgelaufen ist, wird ausscchalten
 *
 * Sinn ist, dass man das Licht laenger anlassen kann, wenn z.B. geputzt
 * oder gearbeitet wird.
 *
 */

typedef struct
{
	uint8_t type;
	eds_lichtzone_block_t config;

	/**
	 * besagt, ob das Licht der Lichtzone an oder aus ist, bzw. ob
	 * sie im extended Mode (nach 3-fach Click ist)
	 */
	uint8_t state; // 0 = off, 1 = on, 2 = on/extended

	/**
	 * Zeit-Zaehler fuer Dinge wie Treppenhaus-Licht o.ae. Schaltungen
	 */
	uint16_t timer_counter;

	/**
	 * wird im Extended-On Modus verwendet: Zaehlt die TASTER_DOWN Meldungen
	 */
	uint8_t exton_taster_down_counter;
	/**
	 * wird im Extended-On Modus verwendet: Zaehlt die Sekunden seit der ersten TASTER_DOWN Meldung
	 */
	uint8_t exton_sec_counter;
} device_data_lichtzone;

void lichtzone_can_callback(device_data_lichtzone *p, 
		const canix_frame *frame);
void lichtzone_timer_handler(device_data_lichtzone *p);

#endif

