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
 *  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */
#ifndef SHIFTIO_H
#define SHIFTIO_H

/** IO-Extension for HCAN */

#include <avr/io.h>

#define DELAY_CYCLES 5 // shift-out-Frequenz-Reduzierung zur Entstoerung

// blau in Datenblatt
#define SHIFT_SCK_bl (1<<PC0) // Clock-Port-Pin fuer In- und Out-Shiftregister
void initShiftInOut(void);


// -----------shiftOUT---74HC595-Shift-Out-Schieberegister--------------------------------// 
#define MAX_NUM_OF_SHIFT_OUT_REGISTER 4// 12 // Maximalausbau: 12 x 8 = 96 Shiftregister-Ausgaenge

// gelb in Datenblatt
#define SHIFT_OUT_RCK_ge (1<<PC2) // Ausgabe-"Freigabe"-Pin

// rosa-orange in Datenblatt
#define SHIFT_OUT_SIN_PIN_or (1<<PC3) // Port-Pin fuer die seriellen Daten zum 74HC595-Shift-Out-Schieberegister

#define SHIFT_INIT_VAL ( SHIFT_OUT_SIN_PIN_or | SHIFT_OUT_RCK_ge | SHIFT_SCK_bl | SHIFT_IN_PS_PIN_gr )
#define SHIFT_DDR DDRC
#define SHIFT_PORT PORTC
#define SHIFT_PORTIN PINC

inline void shiftOut_timer_handler(void);
inline void changeShiftOutPinState(uint8_t n, uint8_t state);
inline uint8_t getShiftOutPinState(uint8_t n);


// -----------shiftIN---4021-Shift-In-Schieberegister-----------------------------------// 
// gruen in Datenblatt
#define SHIFT_IN_SIN_gn (1<<PC1) // Port-Pin fuer die seriellen Daten vom 4021-Shift-In-Schieberegister

// grau (ex. weiss) in Datenblatt
#define SHIFT_IN_PS_PIN_gr (1<<PC4) // Port-Pin fuer die Parallel-/Seriell-Umschaltung

inline uint8_t isBitFromShiftInSet(uint8_t n);



#endif
