#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include <hcan_multicast.h>
#include <lcd.h>
#include <lcdctrl.h>
#include <keys.h>
#include <timer.h>
#include <lcdstatemachine.h>

#include <eds.h>

#define USERPANEL_VERSION "0.9.5"
char userpanel_version[] PROGMEM = "USERPANEL_VERSION_" \
								   USERPANEL_VERSION "_";


void hauselektrik_callback(const canix_frame *frame)
{
	// Dieser Handler wird fuer alle Destination Adressen ausgefuehrt
	// daher muss gefiltert werden, was uns betrifft und was nicht

	if ( (frame->data[0] != HCAN_SRV_HES) || 
			((frame->dst != canix_selfaddr()) && 
			 (frame->dst != HCAN_MULTICAST_CONTROL) &&
			 (frame->dst != HCAN_MULTICAST_INFO))) 
	{
		// Diese Message ist nicht interessant, daher ignorieren
		return;
	}

	lcdstatemachine_can_callback(frame);

	switch (frame->data[1])
	{
		case HCAN_HES_CONFIG_RELOAD :
			return;
	}
}

int main(void)
{
	wdt_enable(WDTO_2S);
	canix_init();

	// Haus-Elektrik Service Handler installieren
	canix_reg_frame_callback(hauselektrik_callback, -1, 
			HCAN_PROTO_SFP, HCAN_SRV_HES);

	canix_reg_rtc_callback(timer_handler);
	canix_reg_idle_callback(idle_handler);

	lcd_init(LCD_DISP_ON);
	lcd_clrscr();

	/*
	 * HINT: Grad Celsius = \337
	 *       (octal fuer 206 bzw. 0xdf)
	 */

	lcdctrl_init();
	keys_init();
	lcdstatemachine_init();

	canix_mainloop();
	return 0;
}

