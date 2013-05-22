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
#include <avr/io.h>
#include <stdint.h>

#include <shiftio.h>
#include <shiftio_debugPort.h>
#include <canix/canix.h>
#include <canix/tools.h>

//-----------shiftOUT---------------------------------------//
static uint8_t data[MAX_NUM_OF_SHIFT_OUT_REGISTER] = {0x00,0x00, 0x00 ,0x00};

inline void doClkOutput(void)
{
	SHIFT_PORT |= SHIFT_SCK_bl; //Clock-Ausgang auf 1 ...
	_delayFourCycles(DELAY_CYCLES);
    //asm volatile ("nop");

    SHIFT_PORT &= ~SHIFT_SCK_bl; //und wieder zurueck auf 0
	_delayFourCycles(DELAY_CYCLES);
    //asm volatile ("nop");
}

//-----------shiftIN / shiftOUT-----------------------------//
void initShiftInOut(void)
{
    SHIFT_DDR |= SHIFT_INIT_VAL; //Ausgangs- bzw. Eingangs-Pins konfigurieren
    SHIFT_PORT &= ~SHIFT_SCK_bl; //Pin auf 0V
    //fuer shift-OUT:
    SHIFT_PORT &= ~SHIFT_OUT_RCK_ge; //Pin auf 0V
    SHIFT_PORT &= ~SHIFT_OUT_SIN_PIN_or; //Pin auf 0V

    //fuer shift-IN:
    SHIFT_PORT &= ~SHIFT_IN_SIN_gn; //Eingangs-Pullup ausschalten
    SHIFT_PORT &= ~SHIFT_IN_PS_PIN_gr; //Pin auf 0V
}

//-----------shiftIN--------------------------------------//
inline void doClkInput(void)
{
	SHIFT_PORT |= SHIFT_SCK_bl; //Clock-Ausgang auf 1 ...
    //asm volatile ("nop");

    SHIFT_PORT &= ~SHIFT_SCK_bl; //und wieder zurueck auf 0
    //asm volatile ("nop");
}

/* Die parallelen Daten ins Shiftregister uebernehmen.
 * Dazu am PS-Eingang am Shiftregister einen 0-1-0 Puls erzeugen */
inline void storeFromParallelInputs(void)
{
    SHIFT_PORT |= SHIFT_IN_PS_PIN_gr; //aktiviert Parallel-Shift
    SHIFT_PORT &= ~SHIFT_IN_PS_PIN_gr; //aktiviert Seriell-Shift
}

//n-tes Bit am shift-register abfragen
inline uint8_t isBitFromShiftInSet(uint8_t n)
{
    storeFromParallelInputs(); //parallele InputDaten in die 4021er einspeichern
    /* Das erste Bit liegt jetzt schon vor. Daher, wenn n=0 ist,
     * wird doClkInput() in der folgenden for-Schleife nicht ausgefuehrt. */
    uint8_t i;
    for (i=0; i<n; i++)
    {
    	doClkInput(); //je ein Bit seriell holen
    }
    return (SHIFT_PORTIN & SHIFT_IN_SIN_gn) != 0; //und das geholte Bit abfragen
}

//-----------shiftOUT---------------------------------------//
/* 8 Bits ins ShiftOut-Register schieben */
inline void shiftOutByte(uint8_t data)
{
    uint8_t i;
    for (i=8; i>0; i--) //8 Bits muessen ausgegeben werden
    {
        //Datenleitung entsprechend gesetzt oder geloescht
        if( (data & 0x80) != 0 ) //MSB gesetzt?
        {
            SHIFT_PORT |= SHIFT_OUT_SIN_PIN_or; //Eine 1 ausgeben
        }
        else
        {
            SHIFT_PORT &= ~SHIFT_OUT_SIN_PIN_or; //Eine 0 ausgeben
        }
        _delayFourCycles(DELAY_CYCLES);
        //asm volatile ("nop");

        doClkOutput(); //einen Clock-Impuls zur Uebernahme des Bits
        data = data << 1;
    }
}

/* "Freigabe:"
 * Die Ausgabedaten im Shiftregister in das Ausgaberegister uebernehmen.
 * Dazu am RCK-Eingang am Shiftregister einen 0-1-0 Puls erzeugen. */
inline void activateNewOutput(void)
{
    SHIFT_PORT |= SHIFT_OUT_RCK_ge; //setzen
    _delayFourCycles(DELAY_CYCLES);
    //asm volatile ("nop");

    SHIFT_PORT &= ~SHIFT_OUT_RCK_ge;
    _delayFourCycles(DELAY_CYCLES);
    //asm volatile ("nop");
}

/* numOfBytesToShiftOut ist abhaengig von der Anzahl
 * der angeschlossenen ShiftOut-Register. */
inline void shiftOutDataArray(uint8_t * pData)
{
    uint8_t i;
    for(i=0; i<MAX_NUM_OF_SHIFT_OUT_REGISTER; i++)
    {
        shiftOutByte(*pData);
        pData--;
    }
    activateNewOutput();
}

/* Die Funktion regelt Ausgabe-Wiederholungen zur
 * Unterdrueckung von ggf. auftretenden Stoerungen
 * beim Ein- oder Abschalten von Verbrauchern.
 * Der shiftOut_handler wird alle 10 ms aufgerufen. */
inline void shiftOut_timer_handler(void)
{
	/* MAX_NUM_OF_SHIFT_OUT_REGISTER -> immer alle Bytes rausschieben.
	 * Geht auch dann, wenn z.B. nur 4 Register angeschlossen sind.
	 * VT: Die SW braucht bei geaenderte shiftregister-Anzahl
	 * nicht veraendert werden. Nur die ohnehin notwendige
	 * Konfiguration im EEPROM ist vorzunehmen. */
	shiftOutDataArray( &data[MAX_NUM_OF_SHIFT_OUT_REGISTER-1] );
}

inline void changeShiftOutPinState(uint8_t n, uint8_t state)
{
	uint8_t i;
	uint8_t j = 0; // 0, 8, 16, ...
	for(i=0; i<MAX_NUM_OF_SHIFT_OUT_REGISTER; i++)
	{
		if ( n < (j+8) ) // gesuchtes data-Byte?
		{
			if (state)
				data[i] |= (1<< (n-j));
			else
			{
				data[i] &= ~(1<< (n-j));
				//canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("%x%x"), n-j, i);
			}
			break;
		}
		j += 8;
	}
}

inline uint8_t getShiftOutPinState(uint8_t n)
{
	uint8_t rtn = 0;
	uint8_t i;
	uint8_t j = 0; // 0, 8, 16, ...
	for(i=0; i<MAX_NUM_OF_SHIFT_OUT_REGISTER; i++)
	{
		if ( n < (j+8) ) // gesuchtes data-Byte?
		{
			rtn = data[i] & (1<< (n-j));
			break;
		}
		j += 8;
	}
	return rtn;
}

