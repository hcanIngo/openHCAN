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
 *
 *
 *  Parts of this code are adapted from Florian Greif's MCP2515 Code; he
 *  released it under the GPL: 
 *  http://www.kreatives-chaos.com/index.php?seite=mcp2515_init
 *
 *  
 *  Parts of this code are adapted from Martin Thomas' MCP2515 Code:
 *
 *****************************************************************************
 *
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 * 
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : mcp2515.c
 * Version : 0.9
 * 
 * Summary : MCP2515 "low-level" driver
 *
 * Parts of this code are adapted from a MCP2510 sample-application 
 * by KVASER AB, http://www.kvaser.com (KVASER-code is marked as free)
 *
 * This code-module is free to use but you have to keep the copyright
 * notice.
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
////#include <avr/signal.h>
#include <avr/wdt.h>

#include "chipdef.h"
#include "mcp2515.h"
#include "mcp2515_defs.h"

#include "canix.h"
#include "tools.h"

#ifdef MCU_atmega8

#define SPI_PORT_DDR DDRB
#define SPI_PORT     PORTB
#define SPI_MISO    4
#define SPI_MOSI    3
#define SPI_SCK     5
#define SPI_CS_PORT_DDR DDRD
#define SPI_CS_PORT     PORTD
#define SPI_CS		3

#elif (MCU_atmega32 || MCU_atmega644)

#define SPI_PORT_DDR DDRB
#define SPI_PORT     PORTB
#define SPI_MISO    6
#define SPI_MOSI    5
#define SPI_SCK     7
#define SPI_CS_PORT_DDR DDRB
#define SPI_CS_PORT     PORTB
#define SPI_CS		4

#else

#error "NO MCU type defined"

#endif

#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)

void spi_init(void)
{
	// Aktivieren der SCK, MISO und MOSI Ports
	// Die CS Ports (ChipSelect) werden von den can_open_interface
	// Aufrufen initialisiert
	
	SPI_PORT_DDR |= (1<<SPI_SCK)|(1<<SPI_MOSI);
	SPI_PORT &= ~(1<<SPI_SCK)|(1<<SPI_MOSI)|(1<<SPI_MISO);

	// SPI Masterinterface aktivieren
	// f(spi) = f(cpu) / 2 (Double-Speed, da wir Master sind)
	
	SPCR = (1<<SPE)|(1<<MSTR);
	SPSR = (1<<SPI2X);
}

uint8_t spi_putc( uint8_t data )
{
    /* Sendet ein Byte */
    SPDR = data;

    /* Wartet bis Byte gesendet wurde */
    while( !( SPSR & (1<<SPIF) ) )
        ;

    return SPDR;
}

uint8_t spi_getc(void)
{
    return spi_putc(0);
}

void can_enable_mcp2515(void)
{
	SPI_CS_PORT &= ~(1<< SPI_CS);
}

void can_disable_mcp2515(void)
{
	SPI_CS_PORT |= (1<< SPI_CS);
}

void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
	can_enable_mcp2515();
    spi_putc(SPI_BIT_MODIFY);
    spi_putc(adress);
    spi_putc(mask);
    spi_putc(data);
	can_disable_mcp2515();
}

uint8_t mcp2515_read_register(const uint8_t address)
{
    uint8_t ret;

	can_enable_mcp2515();
    spi_putc(SPI_READ);
    spi_putc(address);
    ret = spi_getc();
	can_disable_mcp2515();

    return ret;
}

void mcp2515_write_register(uint8_t adress, uint8_t data )
{
	can_enable_mcp2515();
    spi_putc(SPI_WRITE);
    spi_putc(adress);
    spi_putc(data);
	can_disable_mcp2515();
}

uint8_t mcp2515_set_mode(uint8_t mode)
{
	uint8_t i;

	mcp2515_bit_modify( MCP_CANCTRL, MODE_MASK, mode);

	// verify as advised in datasheet
	i = mcp2515_read_register( MCP_CANCTRL);
	i &= MODE_MASK;

	if ( i == mode )
		return MCP2515_OK;
	else
	{
		return MCP2515_FAIL;
	}
}

void mcp2515_write_register_p(uint8_t adress, const uint8_t *data, 
		uint8_t length )
{       
    uint8_t i;
    
	can_enable_mcp2515();

    spi_putc(SPI_WRITE);
    
    spi_putc(adress);

    for (i=0; i<length ;i++ )
        spi_putc(*data++);
    
	can_disable_mcp2515();
}       

void mcp2515_read_register_p(uint8_t adress, uint8_t *data, uint8_t length )
{   
    uint8_t i;

	can_enable_mcp2515();
 
    spi_putc(SPI_READ);

    spi_putc(adress);

    for (i=0; i<length ;i++ )
        *data++ = spi_putc(0xff);

	can_disable_mcp2515();
}

uint8_t can_open_interface(void)
{
	uint8_t res;

	// 1. Aktivieren des CS Pins
	SPI_CS_PORT_DDR |= (1<< SPI_CS);
	SPI_CS_PORT |= (1<< SPI_CS);

	// 2. MCP2515 Software Reset
	can_enable_mcp2515();
	spi_putc(SPI_RESET);
	can_disable_mcp2515();

	delay(255);	

	// Device in den Konfigurations Modus versetzten
	res = mcp2515_set_mode( MODE_CONFIG);
	if (res)
		return res;

	// Speed einstellen: 
	// BRP = F_CAN / (CAN_SPEED*2*8)) - 1
	//
	// d.h. fuer 
	// F_CAN = 8MHz:
	// - CAN_SPEED: 125 KHz: BRP = 3
	// - CAN_SPEED: 250 KHz: BRP = 1
	// - CAN_SPEED: 500 KHz: BRP = 0
	//
	//mcp2515_write_register( CNF1, (1<<BRP0)|(1<<BRP1)|(1<<BRP2));
	mcp2515_write_register( CNF1, (F_CAN / (CAN_SPEED*2*8)) - 1);

	// Prop Seg und Phase Seg1 einstellen
	mcp2515_write_register( CNF2, (1<<BTLMODE)|(1<<PHSEG11) );

	// Wake-up Filter deaktivieren, Phase Seg2 einstelen
	mcp2515_write_register( CNF3, (1<<PHSEG21) );

	// Aktivieren der Rx Buffer Interrupts
	mcp2515_write_register( CANINTE, /*(1<<RX1IE)|(1<<RX0IE)*/ 0);

	// Buffer 0 : Empfangen aller Nachrichten, kein Rollover (siehe unten)
	mcp2515_write_register( RXB0CTRL, (1<<RXM1)|(1<<RXM0));

	/* Buffer 1 : wird nicht verwendet, da der folgender Effekt aufgetreten
	 * ist: Wenn man Buffer 1 + Rollover (BUKT) aktiviert, dann kann es bei
	 * hoher Empfangsframerate passieren, dass der MCP kein Paket mehr
	 * meldet. Nach einem Reset geht es wieder. Effekt tritt auf, wenn man
	 * per HCAN Syslog einen ganzen Stoss an Frames sendet
	 */

	// Deaktivieren der Pins RXnBF Pins ( High Impedance State )
	mcp2515_write_register( BFPCTRL, 0 );

	// TXnRTS Bits als Inputs schalten
	mcp2515_write_register( TXRTSCTRL, 0 );

	// Zurueck in den Normal-Mode
	res = mcp2515_set_mode( MODE_NORMAL);
    if (res)
        return res;
		
	return 0;
}

uint8_t can_send_message(uint32_t id, const uint8_t *data, uint8_t length)
{
	uint8_t tx_index;
	uint8_t temp[4];

	// Herausfinden, welcher Buffer frei ist:
	
	if (can_sendable(0))
		tx_index = 0;
	else if (can_sendable(1))
		tx_index = 1;
	else if (can_sendable(2))
		tx_index = 2;
	else
	{
		canix_tx_dropped++;
		return MCP2515_FAIL;
	}

	cli();

	// Dieser Buffer frei, also hier einfuettern
	//
	// Identifier einstellen
	temp[0] = (uint8_t) (id>>21);
	temp[1] = (((uint8_t) (id>>16)) & 0x03) |
		(((uint8_t) (id>>13)) & 0xE0) | (1<<EXIDE);
	temp[2] = (uint8_t) (id>>8);
	temp[3] = (uint8_t) id;
	mcp2515_write_register_p( TXB0SIDH + (tx_index << 4), temp, 4 );

	// Datenlaenge einstellen
	mcp2515_write_register( TXB0DLC + (tx_index << 4), length );

	// Datenbytes schreiben
	mcp2515_write_register_p( TXB0D0 + (tx_index << 4), data, length );

	// REQ Bit setzen
	mcp2515_bit_modify(TXB0CTRL + (tx_index << 4), (1 << TXREQ), (1<<TXREQ));

	// Nachricht senden
	can_enable_mcp2515();
	spi_putc(SPI_RTS | (1 << tx_index));

	can_disable_mcp2515();

	sei();

	canix_tx_sent++;

	return MCP2515_OK;
}

bool can_sendable(const uint8_t tx_index)
{
	uint8_t v;
	
	cli();
	v = mcp2515_read_register( TXB0CTRL + (tx_index << 4) );
	sei();

	return (! (v & (1 << TXREQ)));
}

bool can_any_sendable(void)
{
	return (can_sendable(0) || can_sendable(1) || can_sendable(2));
}

bool can_message_available(void)
{
	uint8_t n;

	cli();
	n = canix_rxq_size() > 0;
	sei();

	return n;
}

bool mcp2515_message_available(void)
{
	char temp;
	temp = mcp2515_read_register(CANINTF);
	return (temp & ((1 << RX0IF)));
}

// wird NUR vom ISR aufgerufen!! (Daher kein cli()/sei() Paerchen)
uint8_t mcp2515_read_message(uint32_t *extended, char *data)
{
	uint8_t size;
	uint8_t id[4];

	mcp2515_read_register_p( RXB0SIDH, id, 4 );
	*extended = id[3] | 
		(((uint32_t) id[2]) << 8) |
		(((uint32_t) id[1] & 0x03) << 16) | 
		(((uint32_t) id[1] & 0xe0) >> 3 << 16) | 
		(((uint32_t) id[0]) << 21);

	// Frame Payload size
	size = mcp2515_read_register( RXB0DLC) & 0x0F;

	// Daten kopieren
	mcp2515_read_register_p(RXB0D0, (uint8_t *)data, size);

	// RX0 Interrupt Flag loeschen
	mcp2515_bit_modify(CANINTF, 0x01, 0x00 );

	return size;
}

uint8_t can_read_message(uint32_t *extended, char *data)
{
	uint8_t size = 0;
	uint8_t i;

	cli();
	if (canix_rxq_size())
	{
		uint8_t idx; 

		idx = canix_rxq_fetch_one();

		*extended = canix_rx_queue[idx].extended;
		for (i = 0; i < canix_rx_queue[idx].size; i++)
			data[i] = canix_rx_queue[idx].data[i];
		size = canix_rx_queue[idx].size;
		canix_rx_queue[idx].used = 0;
	}
	sei();

	return size;
}

uint16_t can_error_counters(void)
{
	return (mcp2515_read_register(REC) << 8) |
		mcp2515_read_register(TEC);
}

