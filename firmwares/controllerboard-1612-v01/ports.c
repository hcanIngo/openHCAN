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
#include <avr/io.h>
#include <stdint.h>

#include <canix/canix.h>
#include <canix/tools.h>
#include <canix/syslog.h>
#include <hcan.h>
#include <hcan_multicast.h>

#include "ports.h"
#include "i2cmaster.h"
#define MCP23x17_ADDR 0x40 // Adressierung: A2 bis A0 auf GND + WRITE

// control registers
// ONLY VALID WHEN ICON.BANK = 0
#define MCP23x17_IODIRA   0x00 // 8-Bit-Port A (left):  configure port as output
#define MCP23x17_IODIRB   0x01 // 8-Bit-Port B (right): "

#define MCP23x17_GPPUA    0x0C // 8-Bit-Port A (left):  activate pull-up for input
#define MCP23x17_GPPUB    0x0D // 8-Bit-Port B (right): "
#define MCP23x17_INTFA    0x0E // Interrupts...

#define MCP23x17_GPIOA    0x12 // choice the port A to read or write
#define MCP23x17_GPIOB    0x13 // choice the port B to read or write

volatile static uint8_t valuesOfPortsOnExpBoard[MAXIMUM_NUMBER_OF_EXPANDER_BOARDS*4];
volatile static uint8_t previousValuesOfPortsOnExpBoard[MAXIMUM_NUMBER_OF_EXPANDER_BOARDS*4];

static inline void writeMCP23017port (uint8_t port, uint8_t address, uint8_t outputByte);

bool portsDeviceCreated = false;
bool expanderActive = false;
static  uint8_t* expBoard; // z.B.  0,0,15,15 // out,out,in,in (Expander 0 1 2 3); Einzelbits
static uint8_t outBase, inBase;


// Bsp. fuer die FESTLEGUNG der HW-Konfiguration:
//                                 |--------OUTPUT--------|  |---------INPUT--------|
// portindex                        0  1  2  3   4  5  6  7   8  9 10 11  12 13 14 15
// boardindex (fuer expBoard)       0            1            2            3
//              expBoard            0            0           15           15            //     "    ,    "
//          aus expBoard            0  0  0  0   0  0  0  0   1  1  1  1   1  1  1  1   // 0: Output, 1: Input
static uint8_t expStartPins[] =  {12,20,28,36, 44,52,60,68, 16,24,32,40, 48,56,64,72}; // Startpinnummern
// Adressen der MCP23x17            0  1  0  1   2  3  2  3   4  5  4  5   6  7  6  7
// bank 0=A, 1=B                    A  A  B  B   A  A  B  B   A  A  B  B   A  A  B  B


static inline void configurePortAsInput (uint8_t bank, uint8_t adr)
{
	if (0 == i2c_start(MCP23x17_ADDR + (adr<< 1) + I2C_WRITE))
	{
		i2c_write(MCP23x17_IODIRA + bank); // select either IODIRA or IODIRB (via bank)
		i2c_write(0xff); // if selected, the whole bank is set as input
		i2c_stop();

		// activate pull-up for input ports:
		i2c_start(MCP23x17_ADDR + (adr<< 1) + I2C_WRITE);
		i2c_write(MCP23x17_GPPUA + bank); // select either GPPUA or GPPUB via bank
		i2c_write(0xff); // the whole bank gets pull-up
	}
	i2c_stop();
}

static inline void configurePortAsOutput (uint8_t bank, uint8_t adr)
{
	if (0 == i2c_start(MCP23x17_ADDR + (adr<< 1) + I2C_WRITE))
	{
		i2c_write(MCP23x17_IODIRA + bank); // select either IODIRA or IODIRB via bank
		i2c_write(0x00); // if selected, set whole bank as output
	}
	i2c_stop();
}

bool isMCP23x17available (uint8_t boardindex, uint8_t adr)
{
	bool available = false;

	// check for MCP23x17-IC:
    if (0 == i2c_start(MCP23x17_ADDR + (adr<< 1) + I2C_WRITE)) available = true;
    else
    {
    	// gilt hier immer: if (expIOconfig[boardindex] != 255) // Board (2-MCP23x17-IC's) konfiguriert?
    	canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("noExp%d, but configured %d)"), adr, boardindex); // adr 0..7
    	/* Sollte ein Softreset nicht reichen, dann hier muss ggf. einmal
    	   die Versorgungsspannung des MCP23x17 abgeschaltet werden! */
    }

    i2c_stop();
    return available;
}

void configurePorts (void)
{
	uint8_t portindex = 0;
	uint8_t boardindex;
	uint8_t adr = 0;
	uint8_t port;
	uint8_t bank = 0; // A
	uint8_t numberOfInputs = 0;
	uint8_t numberOfOutputs = 0;
	for (boardindex=0; boardindex<MAXIMUM_NUMBER_OF_EXPANDER_BOARDS; boardindex++) // boardindex: Index auf je 4-Ports (ein IO-Board)
	{
		if (expBoard[boardindex] != 255)
		{
			for (port=0; port<4; port++)
			{
				if (isMCP23x17available(boardindex, adr))
				{
					expanderActive = true;
					if ((expBoard[boardindex] & (1<< port)) != 0) // als INPUT konfiguriert?
					{
						canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("i:%d-%d"), portindex, adr);
						expStartPins[portindex] = inBase + 8*numberOfInputs;
						configurePortAsInput (bank, adr);
						numberOfInputs++;
					}
					else // Bit ist 0, dann als OUTPUT konfiguriert:
					{
						canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("o:%d-%d"), portindex, adr);
						expStartPins[portindex] = outBase + 8*numberOfOutputs;
						configurePortAsOutput (bank, adr);
						numberOfOutputs++;
					}
				}

				portindex++;

				if (port==3) bank = 0; // fuer port 0
				if (port==1) {bank = 1; adr--;} // fuer port 2
				else adr++;
			}
		}
		else
		{
			adr += 2; // Board nicht konfiguriert
			portindex += 4;
		}
	}

}

/* Hinweis: Wird diese Funktion ohne Pullups an IN0 und IN1 ausgefuehrt,
   so kommt es zu wdt-Resets. Daher ports-Device nur konfigurieren,
   wenn auch ein IO-Expander angeschlossen ist! */
void ports_init (device_data_ports *p, eds_block_p it)
{
	if (portsDeviceCreated == true)
	{
		canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("Additional ports configuration ignored"));
		// TODO raus: sendHESMessage (1, HCAN_HES_ADDITIONAL_PORTS_DEVICE_IGNORED);
		return;
	}
	portsDeviceCreated = true; // There can be only ONE ports Device
	expanderActive = false;

	i2c_init(); // SDA/SDL-Pins sind ueber I2C-master-Lib ATmegatypabhaengig festgelegt

	uint8_t i;
	for (i=0; i<4*MAXIMUM_NUMBER_OF_EXPANDER_BOARDS; i++)
	{
		valuesOfPortsOnExpBoard[i] = 0;
		previousValuesOfPortsOnExpBoard[i] = 255;
	}

	expBoard = &p->config.expander0; // z.B. 0,255,15,15 (4x OUT, -, 4x IN, 4x IN)

	if (p->config.base == 128)
	{
		// Zur Erzeugung der Startpinnummern (Ingo's Nummerierung):
		outBase = 12;
		inBase = 16;
		// Bsp.:expStartPins[] = {12,20,28,36, 44,52,60,68, 16,24,32,40, 48,56,64,72};
	}
	else // Christoph's aktuelle Nummerierung:
	{
		outBase = inBase = 32;
	}

	configurePorts ();
}

static inline uint8_t readMCP23017port (uint8_t bank, uint8_t adr)
{
	uint8_t inputByte = 0xFF; // Taster nicht betaetigt, aber Reedkontakte offen
	if (0 == i2c_start (MCP23x17_ADDR + adr + I2C_WRITE))
	{
		// read input at port:
		i2c_write(MCP23x17_GPIOA + bank); // select either GPIOA or GPIOB via bank
		i2c_rep_start (MCP23x17_ADDR + adr + I2C_READ);
		inputByte = i2c_readNak ();
	}
	else expanderActive = false; // Ausnahme: EMV-Sicherung
	i2c_stop ();

	return inputByte;
}

// n-tes Bit am port abfragen
/* valuesOfPortsOnExpanderBoard[i] gespeichert, damit diese Routine nicht fuer jedes Bit
   die ein I2C-Read anstossen muss */
inline uint8_t ports_getInput (uint8_t n)
{
	uint8_t portindex = 0;
	uint8_t boardindex;
	uint8_t port;
	for (boardindex=0; boardindex<MAXIMUM_NUMBER_OF_EXPANDER_BOARDS; boardindex++) // boardindex: Index auf je 4-Ports (ein IO-Board)
	{
		if (expBoard[boardindex] != 255) // Board konfiguriert?
		{
			for (port=0; port<4; port++)
			{
				if ((expBoard[boardindex] & (1<< port)) != 0) // als INPUT konfiguriert?
				{
					if (n < (expStartPins[portindex]+8))
					{
						n -= expStartPins[portindex];
						if (port > 1) n = 7 - n; // Port B: Pins sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
						return valuesOfPortsOnExpBoard[portindex] & (1<< n); // Bit abfragen
					}
				}

				portindex++;
			}
		}
		else portindex += 4; // Board nicht konfiguriert
	}

	// Kommt bei Fehlkonfiguration des ports-Device extrem haeufig: canix_syslog_P(SYSLOG_PRIO_DEBUG, PSTR("Exp:InPin %d NG"), n);
	return 1; // Input-Pin nicht gefunden -> Taster nicht betaetigt, aber Reedkontakt offen!
}

inline void handlerForExpanderUpdate (void)
{
	uint8_t portindex = 0;
	uint8_t boardindex;
	uint8_t adr = 0;
	uint8_t port;
	uint8_t bank = 0; // A
	for (boardindex=0; boardindex<MAXIMUM_NUMBER_OF_EXPANDER_BOARDS; boardindex++) // boardindex: Index auf je 4-Ports (ein IO-Board)
	{
		if (expBoard[boardindex] != 255)
		{
			for (port=0; port<4; port++)
			{
				if ((expBoard[boardindex] & (1<< port)) != 0) // als INPUT konfiguriert?
					valuesOfPortsOnExpBoard[portindex] = readMCP23017port (bank, adr<< 1);  // address shifted because of: A2 A1 A0 R/W
				else // als OUTPUT konfiguriert:
				if (previousValuesOfPortsOnExpBoard[portindex] != valuesOfPortsOnExpBoard[portindex])
				{
					writeMCP23017port (bank, adr, valuesOfPortsOnExpBoard[portindex]);
					previousValuesOfPortsOnExpBoard[portindex] = valuesOfPortsOnExpBoard[portindex];
				}

				if (port==3) bank = 0; // fuer port 0
				if (port==1) {bank = 1; adr--;} // fuer port 2
				else adr++;
				portindex++;
			}
		}
		else
		{
			adr += 2; // Board nicht konfiguriert
			portindex += 4;
		}
	}
}

static inline void writeMCP23017port (uint8_t bank, uint8_t adr, uint8_t outputByte)
{
	if (0 == i2c_start (MCP23x17_ADDR + (adr<< 1) + I2C_WRITE)) // shift address because of R/W-Bit: A2 A1 A0 R/W
	{
		i2c_write(MCP23x17_GPIOA + bank);
		i2c_write (outputByte); // write output to port
	}
	i2c_stop ();
}

static inline uint8_t getMCP23017outportpin (uint8_t n, uint8_t * pportindex)
{
	uint8_t portindex = 0;
	uint8_t boardindex;
	uint8_t port;
	for (boardindex=0; boardindex<MAXIMUM_NUMBER_OF_EXPANDER_BOARDS; boardindex++) // boardindex: Index auf je 4-Ports (ein IO-Board)
	{
		if (expBoard[boardindex] != 255)
		{
			for (port=0; port<4; port++)
			{
				if (!((expBoard[boardindex] & (1<< port)) != 0)) // als OUTPUT konfiguriert?
				{
					if (n < (expStartPins[portindex]+8))
					{
						n -= expStartPins[portindex]; // auf den Bereich 0-7 holen
						*pportindex = portindex;
						return (port>1) ? (7 - n) : n; // Port B: Pins sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
					}
				}

				portindex++;
			}
		}
		else portindex += 4; // Board nicht konfiguriert
	}

	canix_syslog_P(SYSLOG_PRIO_ERROR, PSTR("Exp:OutPin %d NG"), n);
	return 0; // Output-Pin nicht gefunden
}

inline void ports_setOutput (uint8_t n, uint8_t state)
{
	uint8_t portindex;

	n = getMCP23017outportpin (n, &portindex);

	if (state)
		valuesOfPortsOnExpBoard[portindex] |= (1<< n);
	else
		valuesOfPortsOnExpBoard[portindex] &= ~(1<< n);
}

inline uint8_t ports_getOutput (uint8_t n)
{
	uint8_t portindex;
	n = getMCP23017outportpin (n, &portindex);

	return valuesOfPortsOnExpBoard[portindex] & (1<< n);
}

