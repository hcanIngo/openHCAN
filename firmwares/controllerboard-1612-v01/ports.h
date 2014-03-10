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
 *  (c) 2014 by Christoph Delfs and Ingo Lages
 */
#ifndef PORTS_DEVICE_H
#define PORTS_DEVICE_H

#include <avr/io.h>

/** IO-Extension for HCAN */
/* Es gibt zwei Board-Typen:
 * 1. IO-Exp32-Board  mit zwei MCP23017 und somit 32 Aus- oder Eingaben
 * 2. IN-Exp64-Board  mit vier MCP23017 und somit 64 Eingaben
 *
 * Ein I2C-Strang hat maximal 8 MCP23017-IC's (= 16 MCP23017-Ports).
 *
 * Die Boards können als Eingaben oder Ausgaben konfiguriert werden:
 * - MCP23017-IC's mit den Adressen 0,1,2,3 werden als Ausgaben initialisiert.
 * - MCP23017-IC's mit den Adressen 4,5,6,7 werden als Eingaben initialisiert.
 *
 * Ausgaenge:
 * - IO-Exp32-Board mit A2=0, A1=0 (0: switch ON) -> Adressen 0,1
 * - IO-Exp32-Board mit A2=0, A1=1 (0: switch ON) -> Adressen 2,3
 *
 * Eingaenge:
 * - IN-Exp64-Board mit A2=0 (Jumper nicht gesteckt) -> Adressen 4,5,6,7
 * - IO-Exp32-Board mit A2=1, A1=0 (0: switch ON) -> Adressen 4,5
 * - IO-Exp32-Board mit A2=1, A1=1 (0: switch ON) -> Adressen 6,7
 * */

/* Achtung:
 * --------
 * Sind Input-Ports > 15 und oder Output-Pins > 11 konfiguriert,
 * so wird versucht der ATmega die I2C-Devices zu erreichen. Wenn an IN0 und IN1 keine
 * 4,7 kOhm - Pullup's montiert sind macht der atmega Wdt-Resets!
 */

#define MAXIMUM_NUMBER_OF_EXPANDER_BOARDS 4

#include <canix/eds.h>
#include <inttypes.h>
#include <eds-structs.h>

typedef struct
{
	uint8_t type;
	eds_ports_block_t config;
	eds_block_p it; // EDS Block Pointer, noetig fuer EDS Schreib-Ops
} device_data_ports;

void ports_init(device_data_ports *p, eds_block_p it);

extern bool portsDeviceCreated;
extern bool expanderActive;

inline uint8_t ports_getInput(uint8_t portNumber);

inline void ports_setOutput(uint8_t n, uint8_t state);
inline uint8_t ports_getOutput(uint8_t n);

inline void handlerForExpanderUpdate(void);

#endif
