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
 *  (c) 2014 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#ifndef SHIFTIO_H
#define SHIFTIO_H

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
 * so wird versucht der atmega die I2C-Devices zu erreichen. Wenn an IN0 und IN1 keine
 * 4,7 kOhm - Pullup's montiert sind macht der atmega Wdt-Resets!
 */


/* 	Details														A2 A1 A0 (Adressbits)
	-- Baugruppe 1: IO-Exp32 --
	Output-Pin's (12...43)
	1. IO-Exp32-IC6	IO0..7  A(12..19) und IO16..23 B(28..35)	y  x  0
	1. IO-Exp32-IC5	IO8..15 A(20..27) und IO24..31 B(36..43)	y  x  1
	mit x=0 und y=0, durch switches ON

	-- Baugruppe 2: IO-Exp32 --
	Output-Pin's (44...81)
	2. IO-Exp32-IC6	IO0..7  A(44..51) und IO16..23 B(66..73)	y  x  0
	2. IO-Exp32-IC5	IO8..15 A(52..59) und IO24..31 B(74..81)	y  x  1
	mit x=1 (switch OFF) und y=0 (switch ON)

	-- Baugruppe 3: IN-Exp64 --    (oder IO-Exp32, wenn dort y=1)
	Input-Pin's (16...79)
	3. IN-Exp64-IC1 IN0..7	 A(23..16) und IN8..15  B(24..31)	z  0  0  // IO-Exp32 mit x=0, y=1
	3. IN-Exp64-IC2 IN16..23 A(39..32) und IN24..31 B(40..47)	z  0  1  // IO-Exp32 mit x=0, y=1
	3. IN-Exp64-IC3 IN32..39 A(55..48) und IN40..47 B(56..63)	z  1  0  // IO-Exp32 mit x=1, y=1
	3. IN-Exp64-IC4 IN48..55 A(71..64) und IN56..63 B(72..79)	z  1  1  // IO-Exp32 mit x=1, y=1
	mit z=1 durch NICHT gesteckten Jumper

	somit koennen folgende C1612-Board-Pins im EDS konfiguriert sein:
	C1612-Board-Output-Pins (schreiben / lesen)
		- 12..19 -> MCP23x17_GPIOA ->							0  0  0    0
		- 20..27 -> MCP23x17_GPIOA ->							0  0  1    1
		- 28..35 -> MCP23x17_GPIOB ->							0  0  0	   0
		- 36..43 -> MCP23x17_GPIOB ->							0  0  1    1

		- 44..51 -> MCP23x17_GPIOA ->							0  1  0    2
		- 52..59 -> MCP23x17_GPIOA ->							0  1  1    3
		- 66..73 -> MCP23x17_GPIOB ->							0  1  0	   2
		- 74..81 -> MCP23x17_GPIOB ->							0  1  1    3

	C1612-Board-In-Pins (lesen)
		- 16..23 -> MCP23x17_GPIOA ->							1  0  0    4
		- 24..31 -> MCP23x17_GPIOA ->							1  0  1    5
		- 32..39 -> MCP23x17_GPIOB ->							1  0  0    4
		- 40..47 -> MCP23x17_GPIOB ->							1  0  1    5
		- 48..55 -> MCP23x17_GPIOA ->							1  1  0    6
		- 56..63 -> MCP23x17_GPIOA ->							1  1  1    7
		- 64..71 -> MCP23x17_GPIOB ->							1  1  0    6
		- 72..79 -> MCP23x17_GPIOB ->							1  1  1    7
*/

#define MAX_NUM_OF_MCP23017_OUTPUT_PORTS 8 // 8 = maximal zwei IO-Exp32-Boards und
#define MAX_NUM_OF_MCP23017_INPUT_PORTS 8  // 8 = maximal ein IN-Exp64-Board

void initMCP23017 (void);

inline void changeShiftOutPinState (uint8_t n, uint8_t state);
inline uint8_t getShiftOutPinState (uint8_t n);
inline void writeAllMCP23017Outputs (void);

inline uint8_t isBitFromShiftInSet (uint8_t n);
inline void readAllMCP23017Inputs (void);

#endif
