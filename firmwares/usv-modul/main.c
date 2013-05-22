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
#include <hcan_eeprom.h>

#include <usvstatemachine.h>
#include <darlingtonoutput.h>
#include <led.h>


#define USVMODUL_VERSION "2.0"
char controller1612_version[] PROGMEM = "USVMODUL_VERSION_" \
										USVMODUL_VERSION "_";


void idle_handler(void)
{
	usv_state_handler();
}

int main(void)
{
	canix_init();

	darlingtonoutput_init();

	led_init();

	// LEDs anpassen: 
	// Netzbetrieb anzeigen:
	led_set(3,10);
	led_set(6,0);
	// Heartbeat aktivieren:
	led_set(1,1);

	canix_reg_frame_callback(usv_can_callback, -1,
			            HCAN_PROTO_SFP, HCAN_SRV_USVS);
	usv_reset_U_stats();
	
	canix_reg_idle_callback(idle_handler);
	canix_reg_rtc_callback(usv_timer_handler);

	canix_mainloop();
	return 0;
}

