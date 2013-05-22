/*
 *  This file is part of the HCAN tools suite.
 *
 *  HCAN is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  HCAN is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with HCAN; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */

#ifndef CANIX_LED_H
#define CANIX_LED_H


#ifdef CANIX_LED_SETUP1
#define CANIX_LED_YELLOW 1
#define CANIX_LED_RED    2
#define LED_PORT PORTB
#define LED_PORT_DDR DDRB
#endif

#ifdef CANIX_LED_SETUP2
#define CANIX_LED_YELLOW 2
#define CANIX_LED_RED    1
#define LED_PORT PORTB
#define LED_PORT_DDR DDRB
#endif

#ifdef CANIX_LED_SETUP3
#define CANIX_LED_YELLOW 2
#define CANIX_LED_RED    1
#define LED_PORT PORTA
#define LED_PORT_DDR DDRA
#endif


#ifdef CANIX_NO_LED_SETUP

#define canix_led_enable(LED)
#define canix_led_disable(LED)
#define canix_led_init_ports()

#else

#define canix_led_enable(LED)  { LED_PORT &= ~ (1<< LED); }
#define canix_led_disable(LED) { LED_PORT |=  1<< LED; }
#define canix_led_init_ports() \
{ \
    LED_PORT_DDR |=  (1<< CANIX_LED_RED) | (1<< CANIX_LED_YELLOW); \
    LED_PORT |= (1<< CANIX_LED_RED) | (1<< CANIX_LED_YELLOW); \
}

#endif

#endif
