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
 *  Adapted for ATmega644:
 *  (c) 2010 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 */

#ifndef CANIX_H
#define CANIX_H

/**
 * @file 	canix.h
 * 
 * @author 	Martin Haller
 * @date	13.3.2006
 *
 * @brief	enth�lt Funktionen des CANIX Frameworks
 *
 * CANIX ist ein Framework, das versucht, alle Gemeinsamkeiten der Firmwares
 * zu umspannen. Dazu gehoeren:
 *
 * - Frame-Empfang
 * - Frame-Versand
 * - lokales Frame-Zustellen
 * - Implementierung des Basis-Protokolls HMS (HCAN Management Service)
 *   mit z.b. Ping, Typ-Abfragen, EEPROM Zugriff, Flash-Zugriff (nur im
 *   Bootloader)
 * - Uptime-Zaehler
 * - RTC mit Datum und Uhrzeit, Empfang und Verarbeitung 
 *   von Zeit-/Datums-Frames
 * - Reset ueber den Watchdog
 * - EEPROM Lese-/Schreib-Zugriff ueber HMS
 *
 *
 * HCAN (steht fuer Haus-/House-CAN) verwendet den CAN-2.0b Extended
 * Identifier. Darin werden eine Protokoll-ID, Quell- und Ziel-Adresse
 * wie folgt codiert:
 *
 * - Bit 0..9: Quell-Adresse
 * - Bit 10..19: Ziel-Adresse
 * - Bit 20..22: Protokoll ID
 *
 * Die oberen Bits werden nicht verwendet, da in den MCP2515 Errata steht,
 * dass eine bestimmte MCP2515 Baureihe (war zum Startpunkt des HCAN-Projektes
 * unklar, ob/wann das behoben wird) einen Fehler beim Frame-Empfang aufweist:
 * Unter gewissen (unklaren, sporadisch auftretenden Umstaenden) sind die oben
 * Bits alle "0". Um diesem Problem von vorn herein aus dem Weg zu gehen, wurden
 * die oberen Bits einfach nicht verwendet.
 *
 */

#include <avr/io.h>
#include <hcan_eeprom.h>

#define true  1
#define false 0

#define bool char

/**
 * Die HCAN Adress-Maske maskiert 10 Bits (Adress-Bereich 0..1023); sie
 * muss fuer die Quell-Adresse um 10 Bits nach links geschoben werden.
 */
#define ADDR_MASK	0x000003ff

/**
 * Die HCAN Protokoll-Maske maskiert 3 Bits (Werte-Bereich 0..7); sie muss
 * zum Maskieren der Protokoll-ID im CAN-2.0b Identifier um 20 Bits nach
 * links geschoben werden.
 */
#define PROTO_MASK	0x00000007

/**
 * Niedrige Prioritaet fuer die CAN Bus Arbitrierung. Dominater Pegel ist
 * low, somit gewinnt im Kollisionsfalle eine 0 gegenueber einer 1
 *
 * HCAN_PRIO_HI gewinnt gegenueber HCAN_PRIO_LOW
 */
#define HCAN_PRIO_LOW 0x03

/**
 * Hohe Prioritaet fuer die CAN Bus Arbitrierung. Dominater Pegel ist
 * low, somit gewinnt im Kollisionsfalle eine 0 gegenueber einer 1.
 *
 * HCAN_PRIO_HI gewinnt gegenueber HCAN_PRIO_LOW
 */
#define HCAN_PRIO_HI  0x00

/**
 * Maximale (feste) Anzahl von zu registierten HCAN-Frame Callbacks
 */
#define MAX_FRAME_CALLBACKS 16

/**
 * Maximale (feste) Anzahl von zu registierten Idle Callbacks
 */
#define MAX_IDLE_CALLBACKS 8

/**
 * Maximale (feste) Anzahl von zu registierten RTC Callbacks (f = 100 Hz)
 */
#define MAX_RTC_CALLBACKS 8

/** 
 * Repreaesentiert ein HCAN Frame. Datenaustausch dieser Struktur:  Die Anzahl
 * der Datenbytes wird im Feld size definiert, data[8] ist immer 8 Bytes gross.
 */
typedef struct
{
	/**
	 * HCAN Quell-Adresse
	 */
	uint16_t src;

	/**
	 * HCAN Ziel-Adresse
	 */
	uint16_t dst;

	/**
	 * HCAN Protokoll
	 */
	uint8_t proto;

	/**
	 * Anzahl der Datenbytes des Frames. Jedes data[ Index > size ] ist 
	 * undefiniert
	 */
	uint8_t size;

	/**
	 * Die Datenbytes des Frames
	 */
	uint8_t data[8];

} canix_frame;

/**
 * Prototyp fuer einen HCAN Frame Callback
 */
typedef void (*canix_frame_callback) (const canix_frame *frame);

/**
 * Prototyp fuer einen Idle Callback
 */
typedef void (*canix_idle_callback) (void);

/**
 * Prototyp fuer einen RTC Callback
 */
typedef void (*canix_rtc_callback) (void);

/**
 * Diese Funktion muss vor dem Verwenden des Canix-Frameworks aufgerufen
 * werden. Hier werden
 * - alle Callbacks initialisiert
 * - die RTC initialisiert
 * - falls definiert, die LEDs aktiviert
 * - der SPI Port initialisiert
 * - der MCP2515 initialisiert (can_open_interface())
 * - verschiedene Callback Handler, die die Grundfunktionen von Canix
 *   zur Verfuegung stellen, registiert
 * - und die Interrupts eingeschaltet
 */
uint8_t canix_init(void);

/**
 * Liefert die eigene HCAN Adresse (ge-cached).
 */
uint16_t canix_selfaddr(void);

/**
 * dst: Wenn != 1, dann gilt als Filter diese Adresse
 */
/**
 * Registriert einen HCAN Frame Callback. Der Client kann damit
 * HCAN Frames empfangen.
 * @param callback die Adresse der Callback Funktion
 * @param dst die Ziel-Adresse (im CAN 2.0b Header), sofern != -1 (!); sonst
 *        ist der Ziel-Adress-Filter nicht aktiv
 * @param proto das Protokoll (im CAN 2.0b Header), sofern != 0 (!)
 * @param srv die Service ID (data[0]), sofern != 0 (!)
 */
void canix_reg_frame_callback(canix_frame_callback callback, 
		int dst, uint8_t proto, uint8_t srv);

/**
 * Registriert einen Idle Callback Handler. Dieser wird immer dann aufgerufen
 * wenn der Prozessor in der Hauptschleifer der Botschaftsverarbeitung
 * vorbeikommt.
 */
void canix_reg_idle_callback(canix_idle_callback callback);

/**
 * Registriert einen RTC Callback Handler. Dieser wird mit einer Frequenz
 * von 100Hz von der Timer0 ISR aufgerufen.
 *
 * ACHTUNG: ein RTC Handler wird direkt von der Timer ISR aus aufgerufen
 * und darf daher keinerlei Code enthalten, der wiederrum Interrupts
 * ausloest. Es ist vergleichbar mit dem Bottom-Teil des Linux-Kernels.
 * Die eigentlichen Aktionen sollten in einem Idle Handler stattfinden
 * (vergleichbar mit dem Topteil des Linux-Kernels.)
 */
void canix_reg_rtc_callback(canix_rtc_callback callback);

/** 
 * Das ist die Main-Loop. Diese muss nach dem Aufruf des canix_init()
 * in der main() Funktion der Firmware aufgerufen werden. Sie kehrt nie
 * zurueck (Endlosschleife).
 */
void canix_mainloop(void);

/**
 * Verarbeitet anstehende Botschaften wie HCAN Frames etc.. Von hier aus
 * werden alle Aufrufe an die diversen Bereiche deligiert (z.B. durch
 * Idle Handler).
 */
void canix_process_messages(void);
void canix_process_messages_withoutIdle(void);

/**
 * Sendet einen HCAN Frame. Dazu wird der Frame an den MCP2515
 * uebergeben und (da dieser zu sendende Frames nicht als Loop
 * zustellt) manuell ueber canix_deliver_frame(..) lokal zugestellt.
 * Damit koennen verschiedene Module der Firmware ebenfalls ueber
 * HCAN Frames kommunizieren.
 *
 * Die Routine uebergibt zum Senden an den MCP2515 das Frame der Funktion
 * @ref canix_frame_send_with_prio mit der Prioritaet HCAN_PRIO_LOW
 */
void canix_frame_send(const canix_frame *frame);

/**
 * Wie @ref canix_frame_send, allerdings mit einer explizit angegebenen
 * Prioritaet.
 * uebergeben und (da dieser zu sendende Frames nicht als Loop
 * zustellt) manuell ueber canix_deliver_frame(..) lokal zugestellt.
 * Damit koennen verschiedene Module der Firmware ebenfalls ueber
 * HCAN Frames kommunizieren.
 *
 * @param frame das zu versendende HCAN Frame
 * @param prio  HCAN_PRIO_LOW, HCAN_PRIO_HI
 */
void canix_frame_send_with_prio(const canix_frame *frame, uint8_t prio);

/**
 * Hier der Watchdog auf 15 msec gesetzt, eine Endlosschleife gestartet und
 * gewartet, bis der Watchdog einen Reset ausloest. Die Zeit bis zum Ausloesen
 * des Watchdog dauert max 15 msec, kann aber auch schneller gehen (je nachdem,
 * wann ein wdt_reset() das letzte Mal stattgefunden hat).
 */
void canix_reset(void);

/**
 * Nur zur internen Verwendung:
 *
 * Stellt einen HCAN Frame (nur!) lokal zu (d.h. es werden die entsprechenden
 * HCAN-Callbacks aufgerufen, sofern die Filter dazu passen).
 *
 * Es gibt 3 Filter:
 * * Source Address (Wildcard ist hier -1; Ausnahme hier, da 0 schon als
 *   Default-Adresse vorkommen kann)
 * * Protokoll (Wildcard ist 0)
 * * Service (Wildcard ist 0)
 */
void canix_deliver_frame(const canix_frame *frame);

/**
 * "Schlaeft" n mal 10msec; arbeitet in der Zeit aber die Main Loop
 * ab. Der Code muss sicherstellen, dass er nicht rekursiv aufgerufen wird!
 */
void canix_sleep_100th(uint8_t n);

uint8_t canix_rxq_size(void);
uint8_t canix_rxq_fetch_one(void);

typedef struct 
{
	uint8_t used;
	uint32_t extended;
	uint8_t size;
	uint8_t data[8];
} canix_rx_queue_entry_t;

#define CANIX_QUEUE_SIZE 12
extern volatile canix_rx_queue_entry_t 
	canix_rx_queue[CANIX_QUEUE_SIZE];


extern volatile uint32_t canix_rx_received;
extern volatile uint32_t canix_rx_lost;
extern volatile uint32_t canix_tx_sent;
extern volatile uint32_t canix_tx_dropped;

#endif
