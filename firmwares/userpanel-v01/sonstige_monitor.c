#include "sonstige_monitor.h"

#include <lcdstatemachine.h>

#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <keys.h>
#include <string.h>
#include <stdio.h>
#include <lcd.h>
#include <lcdctrl.h>
#include <eds.h>
#include <eds-structs.h>
#include <canix/rtc.h>
#include <canix/tools.h>
#include <hcan_multicast.h>
#include <hcan.h>
#include <hcan-def.h>
#include <lcdstatemachine.h>
#include <canix/syslog.h>

volatile uint8_t sonstige_configured = 1; // ersteinmal aktivieren
volatile uint8_t monitor_sonstige_state[24];

uint8_t get_count_sonstige_states(void)
{
	// eingeschaltet zaehlen:
	uint8_t n_powerport_on = 0;
	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (monitor_sonstige_state[i] && (monitor_sonstige_state[i] != 255))
			n_powerport_on++;
	}

	//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("c%d"), n_powerport_on);
	return n_powerport_on;
}

/**
 * Fragt alle konfigurierten Sonstige nach ihrem Status und sammelt
 * die Ergebnisse in monitor_sonstige_state
 */
static void init_monitor_sonstige_request_states(void)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_sonstige_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_SONSTIGE_MISSING);
		sonstige_configured = 0;
		return;
	}

	userpanel_sonstige_t c;
	eeprom_read_block(&c, (it+2), sizeof(c));
	uint8_t i;

	// Ergebnis-Array initialisieren:
	for (i = 0; i < 24; i++)
		monitor_sonstige_state[i] = 255;

	// ueber alle Sonstige iterieren:
	for (i = 0; i < 24; i++)
	{
		if (c.sonstiges[i] != 255) // konfiguriert?
		{
			// Nach Kontakt-Status fragen; die Ergebnisse kommen asynchron
			// ueber den CAN Handler rein
			canix_frame message;
			message.src = canix_selfaddr();
			message.dst = HCAN_MULTICAST_CONTROL;
			message.proto = HCAN_PROTO_SFP;
			message.data[0] = HCAN_SRV_HES;
			message.data[1] = HCAN_HES_POWER_GROUP_STATE_QUERY;
			message.data[2] = c.sonstiges[i]; // die Sonstige-Gruppen-ID
			message.size = 3;
			canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);

			canix_sleep_100th(10); // 100msec Pause
		}
		wdt_reset();
	}
}

static void monitor_insert_sonstige_state(uint8_t gruppe, uint8_t state)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_sonstige_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_SONSTIGE_MISSING);
		return;
	}
	userpanel_sonstige_t c;
	eeprom_read_block(&c, (it+2), sizeof(c)); // Ist ein EEPROM-Zugriff, daher langsamer!

	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (gruppe == c.sonstiges[i])
		{
			//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("g%d %d"), i, state);
			monitor_sonstige_state[i] = state;
		}
	}
}

void monitor_sonstige_can_callback(const canix_frame *frame)
{
	if (!sonstige_configured)
		return;

	switch (frame->data[1])
	{
		case HCAN_HES_BOARD_ACTIVE:
			init_monitor_sonstige_request_states(); // noch Hochlauf, sonst warten wir auf _STATE_CHANGE
			break;

		case HCAN_HES_POWER_GROUP_STATE_REPLAY :
		case HCAN_HES_POWER_GROUP_STATE_INFO :
			monitor_insert_sonstige_state(frame->data[2], frame->data[3]);
			break;
	}
}
