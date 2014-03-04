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
#include <avr/io.h>
#include <stdint.h>

#include <shiftio.h>
#include <shiftio_debugPort.h>
#include <canix/canix.h>
#include <canix/tools.h>
#include <hcan.h>
#include <hcan_multicast.h>

#include "devices.h" // fuer hasInExt, hasOutExt

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

static uint8_t outPortIsLatest = 1; // 1 = MCP23017 output ports written
static uint8_t outPortState[MAX_NUM_OF_MCP23017_OUTPUT_PORTS] = {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00}; // default off
static uint8_t inPortState[MAX_NUM_OF_MCP23017_INPUT_PORTS] = {0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF}; // default off
static inline void sendMessage (uint8_t sender, uint8_t port, uint8_t address, uint8_t data);
static inline void writeMCP23017port (uint8_t port, uint8_t address, uint8_t outputByte);

static void configureMCP23017InputOrOutputPort (uint8_t port, uint8_t address)
{
	if (0 == i2c_start (MCP23x17_ADDR + (address<< 1) + I2C_WRITE))
	{
		if (MCP23x17_GPPUA == port  || MCP23x17_GPPUB == port)
		{
			// activate pull-up for input:
			i2c_write (port);
			i2c_write (0xff);
			i2c_stop ();
			sendMessage (0x77, port, address, 0); // 119 = device accessible, set as input, address of expansion IC
		}
		else if (MCP23x17_IODIRA == port || MCP23x17_IODIRB == port)
		{
			// configure port as output:
			i2c_write (port);
			i2c_write (0x00);
			i2c_stop ();
			sendMessage (0x78, port, address, 0); // 120 = device accessible, set as output, address of expansion IC

			 // switch outputs off:
			if (MCP23x17_IODIRA == port)
				writeMCP23017port (MCP23x17_GPIOA, address, 0x00);
			else writeMCP23017port (MCP23x17_GPIOB, address, 0x00);
		}
		// else  error
	}
	else // failed to access device:
	{
		i2c_stop ();
		sendMessage (0x79, 0xFF, address, 0xFF); // 0x79 = 121 = device access failed , address of expansion IC
	}
}

void initMCP23017 (void)
{
	/* Hinweis: Wird diese Funktion ohne Pullups an IN0 und IN1 ausgefuehrt,
	   so kommt es zu wdt-Resets. Daher C1612-Board -IN-Pins > 15 und -Out-Pins > 12 nur
	   konfigurieren, wenn auch eine IO-Exp. angeschlossen ist. */
	uint8_t address;

	i2c_init(); // SDA/SDL-Pins sind ueber I2C-master-Lib ATmegatypabhaengig festgelegt.

	if (hasOutExt) // es ist mindestens ein IOExtension-Out-Port konfiguriert
	{
		for (address=0; address<4; address++) // output port addresses 0,1,2,3
		{
			configureMCP23017InputOrOutputPort (MCP23x17_IODIRA, address);
			configureMCP23017InputOrOutputPort (MCP23x17_IODIRB, address);
		}
	}

	if (hasInExt) // es ist mindestens ein IOExtension-In-Port konfiguriert
	{
		for (address=4; address<8; address++) // input port addresses 4,5,6,7
		{
			configureMCP23017InputOrOutputPort (MCP23x17_GPPUA, address);
			configureMCP23017InputOrOutputPort (MCP23x17_GPPUB, address);
		}
	}
}

// port = MCP23x17_GPIOA or MCP23x17_GPIOB
static inline uint8_t readMCP23017port (uint8_t port, uint8_t address)
{
	uint8_t inputByte = 0xFF;
	if (0 == i2c_start (MCP23x17_ADDR + (address<< 1) + I2C_WRITE)) // address shifted because of: A2 A1 A0 R/W
	{
		// read input at port:
		i2c_write (port);
		i2c_rep_start (MCP23x17_ADDR+(address<< 1) + I2C_READ);
		inputByte = i2c_readNak ();
	}
	i2c_stop ();

	//sendMessage (0x7A, port, address, inputByte); // 0x7A = 122
	return inputByte;
}

// n-tes Bit am port abfragen
/* inPortState[i] gespeichert, damit diese Routine nicht fuer jedes Bit
   die ein I2C-Read anstoessen muss -> in inPortState[i] pollen */
inline uint8_t isBitFromShiftInSet(uint8_t n)
{
	uint8_t inputByte = 0;
	// inputByte fuer Port B ist zu spiegeln (auch beim IO-Exp32-Board)

	//sendMessage (0x77, 0, 0, n);

	if (n < 24) // 16..23
	{
		inputByte = inPortState[0]; // statt: readMCP23017port (MCP23x17_GPIOA, 4);
		n -= 16; // auf den Bereich 0-7 holen
	}
	else if (n < 32) // 24..31
	{
		inputByte = inPortState[2]; // statt: readMCP23017port (MCP23x17_GPIOA, 5);
		n -= 24; // auf den Bereich 0-7 holen
	}
	else if (n < 40) // 32..39
	{
		inputByte = inPortState[1]; // statt: readMCP23017port (MCP23x17_GPIOB, 4);
		n -= 32; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
	}
	else if (n < 48) // 40..47
	{
		inputByte = inPortState[3]; // statt: readMCP23017port (MCP23x17_GPIOB, 5);
		n -= 40; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
	}
	else if (n < 56) // 48..55
	{
		inputByte = inPortState[4]; // statt: readMCP23017port (MCP23x17_GPIOA, 6);
		n -= 48; // auf den Bereich 0-7 holen
	}
	else if (n < 64) // 56..63
	{
		inputByte = inPortState[6]; // statt: readMCP23017port (MCP23x17_GPIOA, 7);
		n -= 56; // auf den Bereich 0-7 holen
	}
	else if (n < 72) // 64..71
	{
		inputByte = inPortState[5]; // statt: readMCP23017port (MCP23x17_GPIOB, 6);
		n -= 64; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
	}
	else if (n < 80) // 72..79
	{
		inputByte = inPortState[7]; // statt: readMCP23017port (MCP23x17_GPIOB, 7);
		n -= 72; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
	}

	return inputByte & (1<< n); // Bit abfragen
}

// readAllInputs pollen/aufrufen, bevor die devices ihr Inputs abfragen
inline void readAllMCP23017Inputs (void)
{
	inPortState[0] = readMCP23017port (MCP23x17_GPIOA, 4); // todo ggf. nur von bei init erreichbaren Adressen lesen
	inPortState[1] = readMCP23017port (MCP23x17_GPIOB, 4);
	inPortState[2] = readMCP23017port (MCP23x17_GPIOA, 5);
	inPortState[3] = readMCP23017port (MCP23x17_GPIOB, 5);

	inPortState[4] = readMCP23017port (MCP23x17_GPIOA, 6);
	inPortState[5] = readMCP23017port (MCP23x17_GPIOB, 6);
	inPortState[6] = readMCP23017port (MCP23x17_GPIOA, 7);
	inPortState[7] = readMCP23017port (MCP23x17_GPIOB, 7);
}

// port = MCP23x17_GPIOA or MCP23x17_GPIOB
// shift address because of R/W-Bit: A2 A1 A0 R/W
static inline void writeMCP23017port (uint8_t port, uint8_t address, uint8_t outputByte)
{
	if (0 == i2c_start (MCP23x17_ADDR + (address<< 1) + I2C_WRITE))
	{
		// write output to port:
		i2c_write (port);
		i2c_write (outputByte);
	}
	i2c_stop ();

	//sendMessage (0x7B, port, address, outputByte); // 0x7B = 123
}

static inline uint8_t getMCP23017outport (uint8_t n, uint8_t * port, uint8_t * address, uint8_t * i)
{
	if (n < 20)      // 12..19
	{
		*i = 0;
		n -= 12; // auf den Bereich 0-7 holen
		*port = MCP23x17_GPIOA;
		*address = 0;
	}
	else if (n < 28) // 20..27
	{
		*i = 1;
		n -= 20; // auf den Bereich 0-7 holen
		*port = MCP23x17_GPIOA;
		*address = 1;
	}
	else if (n < 36) // 28..35
	{
		*i = 2;
		n -= 28; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins von Port B sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
		*port = MCP23x17_GPIOB;
		*address = 0;
	}
	else if (n < 44) // 36..43
	{
		*i = 3;
		n -= 36; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins von Port B sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
		*port = MCP23x17_GPIOB;
		*address = 1;
	}
	else if (n < 51) // 44..51
	{
		*i = 4;
		n -= 44; // auf den Bereich 0-7 holen
		*port = MCP23x17_GPIOA;
		*address = 2;
	}
	else if (n < 60) // 52..59
	{
		*i = 5;
		n -= 52; // auf den Bereich 0-7 holen
		*port = MCP23x17_GPIOA;
		*address = 3;
	}
	else if (n < 74) // 66..73
	{
		*i = 6;
		n -= 66; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins von Port B sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
		*port = MCP23x17_GPIOB;
		*address = 2;
	}
	else // (n < 82) // 74..81
	{
		*i = 7;
		n -= 74; // auf den Bereich 0-7 holen
		n = 7 - n; // Pins von Port B sind zu spiegeln (0 -> 7, 1 -> 6 etc.)
		*port = MCP23x17_GPIOB;
		*address = 3;
	}

	return n;
}

inline void changeShiftOutPinState (uint8_t n, uint8_t state)
{
	uint8_t address; // unused with writeAllMCP23017Outputs
	uint8_t port;
	uint8_t i;
	n = getMCP23017outport (n, &port, &address, &i);

	if (state)
		outPortState[i] |= (1<< n);
	else
	{
		outPortState[i] &= ~(1<< n);
		//canix_syslog_P(SYSLOG_PRIO_CRITICAL, PSTR("%x%x"), n, i);
	}

	outPortIsLatest = 0; // MCP23017 outputs must be written
	writeMCP23017port (port, address, outPortState[i]); // direkt schreiben ohne writeAllMCP23017Outputs
}

// unused:
inline void writeAllMCP23017Outputs (void)
{
	if (!outPortIsLatest)
	{
		outPortIsLatest = 1; // MCP23017 outputs written

		writeMCP23017port (MCP23x17_GPIOA, 0, outPortState[0]);
		writeMCP23017port (MCP23x17_GPIOA, 1, outPortState[1]);
		writeMCP23017port (MCP23x17_GPIOB, 0, outPortState[2]);
		writeMCP23017port (MCP23x17_GPIOB, 1, outPortState[3]);

		writeMCP23017port (MCP23x17_GPIOA, 2, outPortState[4]);  // ggf todo nur von konfigurierten Adressen lesen
		writeMCP23017port (MCP23x17_GPIOA, 3, outPortState[5]);
		writeMCP23017port (MCP23x17_GPIOB, 2, outPortState[6]);
		writeMCP23017port (MCP23x17_GPIOB, 3, outPortState[7]);
	}
}

inline uint8_t getShiftOutPinState (uint8_t n)
{
	uint8_t address_dummy;
	uint8_t port_dummy;
	uint8_t i;
	n = getMCP23017outport (n, &port_dummy, &address_dummy, &i);

	return outPortState[i] & (1<< n);
}

static inline void sendMessage (uint8_t sender, uint8_t port, uint8_t address, uint8_t data)
{
	canix_frame message;
	message.src = canix_selfaddr();
	message.dst = HCAN_MULTICAST_CONTROL;
	message.proto = HCAN_PROTO_SFP;
	message.data[0] = HCAN_SRV_HES;
	message.data[1] = sender; // Absender, z.B. HCAN_HES_MCP23017_PORTA
	message.data[2] = port;
	message.data[3] = address;
	message.data[4] = data;
	message.size = 5;
	canix_frame_send_with_prio(&message, HCAN_PRIO_HI);
}

