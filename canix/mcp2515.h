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
 *  Parts of this code are adapted from Florian Greif's MCP2515 Code;
 *  he released it under the GPL:
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

/**
 * @file 	mcp2515.h
 * 
 * @author 	Martin Haller
 * @author 	Fabian Greif
 * @date	13.3.2006
 *
 * @brief	CAN Treiber Routinen fuer den MCP2515
 *
 * Details zum Ansteuern des MCP2515 ueber SPI finden sich z.B. auf
 * http://www.kreatives-chaos.com/index.php?seite=mcp2515_init
 *
 */
#ifndef _MCP2515_H_
#define _MCP2515_H_ 1

#include "mcp2515_bl.h"

#include "canix.h"

/**
 * Initialisiert das SPI Interface des Atmega; z.Z. ist
 * f(spi) = f(cpu) / 4
 */
void spi_init(void);

/**
 * schreibt ein Byte an den SPI Ausgang und liest zeitgleich
 * eines ein.
 * @return das gelesene Byte
 */
uint8_t spi_putc( uint8_t data );

/**
 * Liest ein Byte ein und schreibt zeitgleich 0
 * @return das gelesene Byte
 */
uint8_t spi_getc(void);

/**
 * Aktiviert den MCP2515 ueber das CS Signal
 */
void can_enable_mcp2515(void);

/**
 * Deaktiviert den MCP2515 ueber das CS Signal
 */
void can_disable_mcp2515(void);

/**
 * Modifiziert ein Register im MCP2515:
 *
 * @param address die Adresse des MCP Registers
 * @param mask die Maske der zu aendernden Bits
 * @param data die neuen Bits, die gesetzt/geloescht werden sollen
 *
 * Alle Bits, die in mask gesetzt sind, werden aus data uebernommen.
 */
void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data);

/**
 * Liest ein Byte aus einem Register des MCP2515.
 */
uint8_t mcp2515_read_register(const uint8_t address);

/**
 * Schreibt ein Byte in ein Register des MCP2515.
 */
void mcp2515_write_register(uint8_t adress, uint8_t data );

/**
 * Versetzt den MCP2515 in den gegebenen Modus.
 * @param mode MODE_NORMAL,MODE_SLEEP, MODE_LOOPBACK, 
 * MODE_LISTENONLY, MODE_CONFIG, MODE_POWERUP
 *
 * @return MCP2515_OK, falls erfolgreich, oder MCP2515_FAIL, falls
 *         fehlgeschlagen
 */
uint8_t mcp2515_set_mode(uint8_t mode);

/*      
 * Beschreibt mehrere Register des MCP2515 auf einmal
 * @param adress Adresse des ersten Registers
 * @param data Zeiger auf die Quell-Daten
 * @param length Anzahl der zu kopierenden Bytes
 */
void mcp2515_write_register_p(uint8_t adress, const uint8_t *data, 
		uint8_t length );

/*      
 * Liest mehrere Register des MCP2515 auf einmal
 * @param adress Adresse des ersten Registers
 * @param data Zeiger auf die Quell-Daten
 * @param length Anzahl der zu kopierenden Bytes
 *
 * ACHTUNG: in data muessen mindestens length Bytes Platz sein !
 */
void mcp2515_read_register_p(uint8_t adress, uint8_t *data, uint8_t length );

/**
 * Initialisiert das MCP2515 CAN Interface. 
 * Dazu wird:
 * - das SPI Interface aktiviert
 * - der MCP Modus auf Config gesetzt
 * - die Bit-Rate-Prescaler gesetzt (z.Z. 125kbit)
 * - RX0 und TX0 aktiviert
 * - alle Filter deaktiviert
 * - in den Normal-Modus zurueckgeschaltet
 *
 * @return MCP2515_OK oder MCP2515_FAIL
 */
uint8_t can_open_interface(void);

/**
 * sendet ein CAN Frame; falls der MCP2515 nicht in 1 Sekunde frei ist
 * bricht er mit MCP2515_FAIL ab.
 *
 * @return MCP2515_OK (0) oder MCP2515_FAIL (1)
 */
uint8_t can_send_message(uint32_t id, const uint8_t *data, uint8_t length);

/**
 * Liefert != 0, wenn in der RX Queue mindestens ein Frame ist 
 */
bool can_message_available(void);

/**
 * Liefert != 0, wenn der MCP2515 einen Frame emfpangen hat und dieser
 * auslesebereit ist
 */
bool mcp2515_message_available(void);

/**
 * Liefert != 0, wenn der MCP2515 einen TX Buffer zum Senden frei hat
 */
bool can_sendable(const uint8_t tx_index);

bool can_any_sendable(void);

/**
 * Liest ein Frame aus der RX Queue.
 *
 * return Anzahl der Datenbytes
 */
uint8_t can_read_message(uint32_t *extended, char *data);

/**
 * Liest das Frame in die extended und data Variablen und loescht es
 * auf dem MCP2515. Im data[] Array muessen mind. 8 Bytes frei sein
 *
 * return Anzahl der Datenbytes
 */
uint8_t mcp2515_read_message(uint32_t *extended, char *data);

/**
 * Liefert die beiden Error Counter (REC und TEC, siehe MCP2515 Datenblatt)
 * REC ist im MSB, TEC im LSB. Werte ueber 128 bedeuten, dass der MCP2515
 * im Passive Error State ist
 */
uint16_t can_error_counters(void);

#endif // _MCP2515_H_
