/*
 *  This file is part of openHCAN.
 *
 *  openHCAN is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  openHCAN is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with openHCAN.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Based on Original Sources:
 *
 *  light weight WS2812 lib
 *  Controls WS2811/WS2812/WS2812B RGB-LEDs
 *  Author: Tim (cpldcpu@gmail.com)
 *  https://github.com/cpldcpu/light_ws2812
 *
 */
#include "ws2812b.h"

#include <canix/syslog.h>
#include <hcan.h>

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <hcan.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

/*
  This routine writes an array of bytes with RGB values to the Dataout pin
  using the fast 800kHz clockless WS2811/2812 protocol.
*/

// Timing in ns
#define w_zeropulse   350  // 0 bit: 0.40us HI, 0.85us LO
#define w_onepulse    900  // 1 bit: 0.80us HI, 0.45us LO
#define w_totalperiod 1250

// Fixed cycles used by the inner loop
#define w_fixedlow    2
#define w_fixedhigh   4
#define w_fixedtotal  8

// Insert NOPs to match the timing, if possible                           @ 16 MHz
#define w_zerocycles    (((F_CPU/1000)*w_zeropulse          )/1000000) // 16000* 350 / 1000000 =  5.6 Zyklen
#define w_onecycles     (((F_CPU/1000)*w_onepulse    +500000)/1000000) // 16000* 900 / 1000000 = 14.4 Zyklen
#define w_totalcycles   (((F_CPU/1000)*w_totalperiod +500000)/1000000) // 16000*1250 / 1000000 = 20   Zyklen

// w1 - nops between rising edge and falling edge - low
#define w1 (w_zerocycles-w_fixedlow)          //  5.6 - 2             = 3.6 Zyklen

// w2   nops between fe low and fe high
#define w2 (w_onecycles-w_fixedhigh-w1)       // 14.4 - 4 - 3.6       = 6.8 Zyklen

// w3   nops to complete loop
#define w3 (w_totalcycles-w_fixedtotal-w1-w2) // 20   - 8 - 3.6 - 6.8 = 1.6 Zyklen

#if w1>0
  #define w1_nops w1
#else
  #define w1_nops  0
#endif

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
#define w_lowtime ((w1_nops+w_fixedlow)*1000000)/(F_CPU/1000)
#if w_lowtime>550
   #error "Light_ws2812: Sorry, the clock speed is too low. Did you set F_CPU correctly?"
//#elif w_lowtime>450
//   #warning "Light_ws2812: The timing is critical and may only work on WS2812B, not on WS2812(S)."
//   #warning "Please consider a higher clockspeed, if possible"
#endif

#if w2>0
#define w2_nops w2
#else
#define w2_nops  0
#endif

#if w3>0
#define w3_nops w3
#else
#define w3_nops  0
#endif

#define w_nop1  "nop      \n\t"
#define w_nop2  "rjmp .+0 \n\t"
#define w_nop4  w_nop2 w_nop2
#define w_nop8  w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8

/*
 * The functions take a byte-array and send to the data output as WS2812 bitstream.
 * The length is the number of bytes to send - three per LED.
 */
static void inline ws2812_sendarray_mask(uint8_t *data, uint16_t datlen, uint8_t pin_maskhi)
{
	uint8_t *dataStart = data;
	uint8_t ctr;

	uint8_t masklo	= ~pin_maskhi & ws2812_port;
	pin_maskhi |= ws2812_port;
	uint8_t sreg_prev = SREG;
	cli(); // Interrupts sperren

	while (datlen--)
	{
		uint8_t curbyte = *data++;
		if (datlen % (maxLedStructLen*3) == 0) data = dataStart; // wdh  (z.B. maxLedStructLen = 30  und  p->config.anzLEDs = 300)
		// Hinweis: Ist jede 3. ausgewaehlt und anzLEDs=3, dann leuchtet nur die erste LED ab "Einspeisung".

		asm volatile(
		"       ldi   %0,8  \n\t"
		"loop%=:            \n\t"
		"       out   %2,%3 \n\t"    //  '1' [01] '0' [01] - re
#if (w1_nops&1)
w_nop1
#endif
#if (w1_nops&2)
w_nop2
#endif
#if (w1_nops&4)
w_nop4
#endif
#if (w1_nops&8)
w_nop8
#endif
#if (w1_nops&16)
w_nop16
#endif
		"       sbrs  %1,7  \n\t"    //  '1' [03] '0' [02]
		"       out   %2,%4 \n\t"    //  '1' [--] '0' [03] - fe-low
		"       lsl   %1    \n\t"    //  '1' [04] '0' [04]
#if (w2_nops&1)
  w_nop1
#endif
#if (w2_nops&2)
  w_nop2
#endif
#if (w2_nops&4)
  w_nop4
#endif
#if (w2_nops&8)
  w_nop8
#endif
#if (w2_nops&16)
  w_nop16
#endif
		"       out   %2,%4 \n\t"    //  '1' [+1] '0' [+1] - fe-high
#if (w3_nops&1)
w_nop1
#endif
#if (w3_nops&2)
w_nop2
#endif
#if (w3_nops&4)
w_nop4
#endif
#if (w3_nops&8)
w_nop8
#endif
#if (w3_nops&16)
w_nop16
#endif
		"       dec   %0    \n\t"    //  '1' [+2] '0' [+2]
		"       brne  loop%=\n\t"    //  '1' [+3] '0' [+4]
		:	"=&d" (ctr)
		:	"r" (curbyte), "I" (_SFR_IO_ADDR(ws2812_port)), "r" (pin_maskhi), "r" (masklo)
		);
	}

	sei();
	SREG = sreg_prev; // somit kann "sei()" entfallen
}

static void inline ws2812_setleds_pin(struct cRGB *ledarray, uint16_t leds, uint8_t pinmask)
{
	ws2812_portReg |= pinmask; // Enable DDR: Output Modus setzen
	ws2812_sendarray_mask((uint8_t*)ledarray, leds+leds+leds, pinmask);
	_delay_us(50); // mindestens 50µs
}

static void inline ws2812_setleds(device_data_ws2812b *p)
{
	uint8_t port = p->config.port;
	if (p->config.port > 1) port++; // INT@D2 frei lassen:  2 -> PD3=D3
	ws2812_setleds_pin(p->sLED, p->config.anzLEDs, _BV(port)); // _BV(bit)  (1 << (bit)) // Converts a bit number into a byte value
}

static void setLedColor(uint8_t i, device_data_ws2812b *p, const uint8_t color, uint8_t value)
{
	*((uint8_t*)&(p->sLED)[i] + color) = value;
}

static uint8_t getLedColor(uint8_t i, device_data_ws2812b *p, const uint8_t color)
{
	return *((uint8_t*)&(p->sLED)[i] + color);
}

static void setAll(device_data_ws2812b *p, uint8_t intensityR, uint8_t intensityG, uint8_t intensityB, uint8_t useLEDs, uint8_t unusedLEDs)
{
	if (useLEDs == 0) return;

	p->status = 0;

	uint8_t iMax = p->config.anzLEDs > maxLedStructLen  ? maxLedStructLen : p->config.anzLEDs; // Begrenzung auf maxLedStructLen
	for (uint8_t i = 0; i < iMax; i++)
	{
		if (i % useLEDs == 0)
		{
		   setLedColor(i, p, COLOR_RED,   intensityR);
		   setLedColor(i, p, COLOR_GREEN, intensityG);
		   setLedColor(i, p, COLOR_BLUE,  intensityB);
		}
		else if (unusedLEDs == 0) // andere LED's ausschalten
		{
		   setLedColor(i, p, COLOR_RED,   0);
		   setLedColor(i, p, COLOR_GREEN, 0);
		   setLedColor(i, p, COLOR_BLUE,  0);
		}
		// else: andere LED's unveraendert

		if (getLedColor(i, p, COLOR_RED) > 0 || getLedColor(i, p, COLOR_GREEN) > 0 || getLedColor(i, p, COLOR_BLUE) > 0)
		{
			p->status = 1;
		}
	}

	ws2812_setleds(p);
}

static void setOneColor(device_data_ws2812b *p, uint8_t color, uint8_t intensity, uint8_t useLEDs, uint8_t unusedLEDs)
{
	if (useLEDs == 0) return;

	p->status = 0;

	uint8_t iMax = p->config.anzLEDs > maxLedStructLen ? maxLedStructLen : p->config.anzLEDs; // Begrenzung auf maxLedStructLen
	for (uint8_t i = 0; i < iMax; i++)
	{
		if (i % useLEDs == 0)
		{
		   setLedColor(i, p, color, intensity);
		}
		else if (unusedLEDs == 0) // andere LED's ausschalten
		{
		   setLedColor(i, p, color, 0);
		}
		// else: andere LED's unveraendert

		if (getLedColor(i, p, COLOR_RED) > 0 || getLedColor(i, p, COLOR_GREEN) > 0 || getLedColor(i, p, COLOR_BLUE) > 0)
		{
			p->status = 1;
		}
	}

	ws2812_setleds(p);
}

inline void ws2812b_timer_handler(device_data_ws2812b *p, uint8_t zyklus)
{
	if (zyklus != 1) return; // 1s-Zyklus verwendet
}

static void ws2812b_toggle(device_data_ws2812b *p)
{
	if (p->status) // ws2812b ist aktiv
	{
		setAll(p, 0, 0, 0, 1, 0);
	}
	else
	{
		setAll(p, 255, 255, 255, 1, 0);
	}
}

static uint8_t is_in_group(const device_data_ws2812b *p, uint8_t group)
{
	uint8_t i;
	uint8_t *gruppen;
	uint8_t maxDeviceGruppen = MAX_WS2812B_GROUPS;

	// die 255 ist der Ersatzwert und wird ignoriert!
	if (group == 255)
			return 0;

	gruppen = (uint8_t *) &(p->config.gruppe0);

	for (i = 0; i < maxDeviceGruppen; i++)
	{
			if (gruppen[i] == group)
					return 1;
	}

	return 0;
}

void ws2812b_init(device_data_ws2812b *p, eds_block_p it)
{
	setAll(p, 0, 0, 0, 1, 0); // einschalten
	p->mute = 0;
	p->poti_farbe = 0;
}

void ws2812b_can_callback(device_data_ws2812b *p, const canix_frame *frame)
{
	canix_frame answer;
	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HES;

	if (is_in_group(p, frame->data[2]))
	{
		switch (frame->data[1])
		{
			case HCAN_HES_WS2812B_SEND :
				if (frame->data[3] == 1) // alle LEDs auf eine Farbe setzen. Nicht gewaehlte LEDs aus
				{
					if (!p->mute) setAll(p, frame->data[4], frame->data[5], frame->data[6], frame->data[7], 0);
				}
				else if (frame->data[3] == 2) // alle LEDs auf eine Farbe setzen. Nicht gewaehlte LEDs bleiben wie sie sind
				{
					if (!p->mute) setAll(p, frame->data[4], frame->data[5], frame->data[6], frame->data[7], 1);
				}
				break;

			case HCAN_HES_TASTER_DOWN :
				if (!p->mute) ws2812b_toggle(p);
				break;

			case HCAN_HES_POWER_GROUP_ON :
			case HCAN_HES_SCHALTER_ON :
				if (!p->mute) setAll(p, 255, 255, 255, 1, 0); // es soll eingeschaltet werden
				break;

			case HCAN_HES_POWER_GROUP_OFF :
			case HCAN_HES_SCHALTER_OFF :
				if (!p->mute) setAll(p, 0, 0, 0, 1, 0); // es soll abgeschaltet werden
				break;

			case HCAN_HES_POWER_GROUP_STATE_QUERY :
				answer.data[1] = HCAN_HES_POWER_GROUP_STATE_REPLAY;
				answer.data[2] = frame->data[2];
				answer.data[3] = p->status;
				answer.data[4] = 0;
				answer.size = 5;
				canix_frame_send_with_prio(&answer,HCAN_PRIO_HI);
				break;
		}
	}
	else if (HCAN_HES_DEVICE_STATES_REQUEST == frame->data[1])
	{
		if (p->config.gruppe0 != 255)
		{
			wdt_reset();
			canix_sleep_100th(10); // 100msec Pause

			answer.data[1] = HCAN_HES_POWER_GROUP_STATE_REPLAY;
			answer.data[2] = p->config.gruppe0; // wird in main.c fuer jedes Device einmal aufgerufen
			answer.data[3] = p->status;
			answer.data[4] = 0;
			answer.size = 5;
			canix_frame_send_with_prio(&answer,HCAN_PRIO_HI);
		}
	}
	else if (frame->data[1] == HCAN_HES_POTI_POS_CHANGED)
	{
		uint16_t adcValue = (frame->data[3] << 8) + frame->data[4];
		if (frame->data[2] == p->config.poti_farb_gruppe)
		{
			if (adcValue <= 256)
			{
				p->poti_farbe = COLOR_RED;
			}
			else if(adcValue <= 512)
			{
				p->poti_farbe = COLOR_GREEN;
			}
			else if(adcValue <= 768)
			{
				p->poti_farbe = COLOR_BLUE;
			}
			else
			{
				p->poti_farbe = COLOR_WHITE;
			}
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("POTI COLOR: %d"), p->poti_farbe);
		}
		else if (frame->data[2] == p->config.poti_helligkeits_gruppe)
		{
			uint8_t helligekeit = (adcValue >> 2);
			if (p->config.feature & (1<<WS2812B_FEATURE_ONLYWHITE)) p->poti_farbe = COLOR_WHITE;
			if (p->poti_farbe == COLOR_WHITE)
			{
				if (!p->mute) setAll(p, helligekeit, helligekeit, helligekeit, 1, 0); // Weiss
			}
			else
			{
				if (!p->mute) setOneColor(p, p->poti_farbe, helligekeit, 1, 0); // Einzelfarbe aendern/setzen
			}
			canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("POTI COLOR: %d POTI INTENSITY: %d"), p->poti_farbe, helligekeit);
		}
	}

	if (p->config.mute == frame->data[2])
	{
		if (HCAN_HES_MUTE_OFF == frame->data[1])
			p->mute = 0; // ws2812b aktiv
		else if (HCAN_HES_MUTE_ON == frame->data[1])
			p->mute = 1; // ws2812b passiv (per Taster nicht aenderbar)
	}
}
