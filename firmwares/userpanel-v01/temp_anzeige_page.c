#include <temp_anzeige_page.h>
#include <lcdstatemachine.h>

#include <canix/syslog.h>

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

volatile uint8_t temperature_expected = 0;
volatile int16_t temperature;

uint8_t temp_anzeige_page_get_temp(eds_temp_anzeige_page_block_t *p,
		int16_t *temp)
{
	canix_frame message;

	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_INFO;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = HCAN_HES_1WIRE_TEMPERATURE_QUERY;
	message.data[2] = p->tempsensor_id;
	message.size = 3;
	canix_frame_send_with_prio(&message, HCAN_PRIO_LOW);
	temperature_expected = 1;

	// Auf die Anwort warten
	canix_sleep_100th(2);

	if (temperature_expected == 0)
	{
		// aha, es ist eine Antwort eingetroffen...
		*temp = temperature;
		return 0;
	}

	// Timeout... keine Antwort erhalten :-(
	return 1;
}

void temp_anzeige_page_handle_key_down_event(
		eds_temp_anzeige_page_block_t *p, uint8_t key)
{
	if (key == KEY_ESC)
	{
		goto_page_up();
	}
}

void temp_anzeige_page_print_page(eds_temp_anzeige_page_block_t *p)
{
	char s[32];
	int16_t Tist;

	// LCD Bildschirm loeschen
	lcd_clrscr();

	// 1. Zeile
	snprintf_P(s,sizeof(s)-1,PSTR("%s"),p->name);
	lcd_gotoxy(0,0);
	lcd_puts(s);

	if (temp_anzeige_page_get_temp(p,&Tist) == 0)
	{
		// /16: Skalieren bei 4 Nachkommastellen-Bits (DS18B20)
		// +8: Runden:
		//
		// Beispiel: 
		// - 19.375 + 0.5 = 19.875; int(19.875) = 19
		// - 19.5625 + 0.5 = 20.0625; int(20.0625) = 20
		snprintf_P(s,sizeof(s)-1,PSTR("T=%d\337C"),(Tist+8) >> 4);
		lcd_gotoxy(0,1);
		lcd_puts(s);
	}
	else
		canix_syslog_P(SYSLOG_PRIO_ERROR, 
				PSTR("temp_anzeige: tempsensor replay %d missing"),
				p->tempsensor_id);
}

void temp_anzeige_page_can_callback(eds_temp_anzeige_page_block_t *p,
		const canix_frame *frame)
{
	switch (frame->data[1])
	{
		case HCAN_HES_1WIRE_TEMPERATURE_REPLAY : 
			{
				if (frame->data[2] == p->tempsensor_id)
				{
					temperature = (frame->data[3] << 8) | frame->data[4];
					temperature_expected = 0;
				}
			}
			break;
	}
}
