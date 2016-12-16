#include "mute_monitor.h"

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

volatile uint8_t mute_configured = 1; // ersteinmal aktivieren
volatile uint8_t monitor_mute_state[24];

uint8_t get_count_mute_states(void)
{
	// eingeschaltet zaehlen:
	uint8_t n_passiv = 0;
	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (monitor_mute_state[i] && (monitor_mute_state[i] != 255))
			n_passiv++;
	}

	//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("c%d"), n_passiv);
	return n_passiv;
}

static void monitor_insert_mute_state(uint8_t gruppe, uint8_t state)
{
	eds_block_p it = eds_find_next_block((eds_block_p)0, EDS_userpanel_mute_BLOCK_ID);
	if (!it)
	{
		load_error_page(ERROR_CONF_MUTE_MISSING);
		return;
	}
	userpanel_mute_t c;
	eeprom_read_block(&c, (it+2), sizeof(c)); // Ist ein EEPROM-Zugriff, daher langsamer!

	uint8_t i;
	for (i = 0; i < 24; i++)
	{
		if (gruppe == c.mute[i])
		{
			//canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("g%d %d"), i, state);
			monitor_mute_state[i] = state;
		}
	}
}

static void init_monitor_mute(void)
{
	// Ergebnis-Array initialisieren:
	uint8_t i;
	for (i = 0; i < 24; i++)
		monitor_mute_state[i] = 255;
}

void monitor_mute_can_callback(const canix_frame *frame)
{
	if (!mute_configured)
		return;

	switch (frame->data[1])
	{
		case HCAN_HES_BOARD_ACTIVE:
			init_monitor_mute();
			// Schalter muessen wir nicht abfragen, diese melden ihren Mute-State bereits
			break;

		case HCAN_HES_MUTE_ON :
			monitor_insert_mute_state(frame->data[2], 1);
			break;

		case HCAN_HES_MUTE_OFF :
			monitor_insert_mute_state(frame->data[2], 0);
			break;
	}
}
