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

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <canix/canix.h>
#include <canix/rtc.h>
#include <canix/mcp2515.h>

#ifdef CANIX_SYSLOG
#include <canix/syslog.h>
#endif

#include <hcan.h>

#ifndef CANIX_HMS
#error "CANIX_HMS muss definiert werden"
#endif

void canix_SFP_HMS_handler(const canix_frame *frame)
{
	canix_frame answer;

	answer.src = canix_selfaddr();
	answer.dst = frame->src;
	answer.proto = HCAN_PROTO_SFP;
	answer.data[0] = HCAN_SRV_HMS;


	switch (frame->data[1])
	{
		case HCAN_HMS_PING_REQUEST :
			answer.data[1] = HCAN_HMS_PING_REPLAY;
			answer.size = 2;
			canix_frame_send(&answer);
			break;
		case HCAN_HMS_DEVICE_STATE_REQUEST :
			answer.data[1] = HCAN_HMS_DEVICE_STATE_REPLAY;
			answer.data[2] = HCAN_HMS_DEV_STATE_APPLICATION;
			answer.size = 3;
			canix_frame_send(&answer);
			break;
		case HCAN_HMS_DEVICE_RESET :
			canix_reset();
			break;
		case HCAN_HMS_DEVICE_TYPE_REQUEST :
			answer.data[1] = HCAN_HMS_DEVICE_TYPE_REPLAY;
#if defined (__AVR_ATmega8__)
			answer.data[2] = 0;
#elif defined (__AVR_ATmega32__)
			answer.data[2] = 1;
#elif defined (__AVR_ATmega644__)
			answer.data[2] = 2;
#endif
			answer.data[3] = eeprom_read_byte((uint8_t *)EEPR_BOARD_TYPE);
			answer.size = 4;
			canix_frame_send(&answer);
			break;
		case HCAN_HMS_INTERNAL_EEPROM_WRITE :
			{
				// only write to eeprom, if the value changed !
				uint8_t read = 
					eeprom_read_byte((uint8_t *)(frame->data[2] |
								(frame->data[3] << 8)));

				if (read != frame->data[4])
					eeprom_write_byte((uint8_t *)(frame->data[2] |
								(frame->data[3] << 8)), frame->data[4]);

				answer.data[1] = HCAN_HMS_INTERNAL_EEPROM_WRITE_ACK;
				answer.size = 2;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_INTERNAL_EEPROM_READ_REQUEST :
			answer.data[1] = HCAN_HMS_INTERNAL_EEPROM_READ_REPLAY;
			answer.data[2] = eeprom_read_byte((uint8_t *)(frame->data[2] |
						(frame->data[3] << 8)));
			answer.size = 3;
			canix_frame_send(&answer);
			break;
		case HCAN_HMS_INTERNAL_EEPROM_READ_BULK_REQUEST :
			{
				uint16_t address;
				address = frame->data[2] | (frame->data[3] << 8);
				answer.data[1] = HCAN_HMS_INTERNAL_EEPROM_READ_BULK_REPLAY;
				answer.data[2] = eeprom_read_byte((uint8_t*) (address + 0));
				answer.data[3] = eeprom_read_byte((uint8_t*) (address + 1));
				answer.data[4] = eeprom_read_byte((uint8_t*) (address + 2));
				answer.data[5] = eeprom_read_byte((uint8_t*) (address + 3));
				answer.data[6] = eeprom_read_byte((uint8_t*) (address + 4));
				answer.data[7] = eeprom_read_byte((uint8_t*) (address + 5));
				answer.size = 8;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_UPTIME_QUERY :
			answer.size = 6;
			answer.data[1] = HCAN_HMS_UPTIME_REPLAY;
			answer.data[2] = canix_rtc_clock.uptime_sec >> 24;
			answer.data[3] = canix_rtc_clock.uptime_sec >> 16;
			answer.data[4] = canix_rtc_clock.uptime_sec >> 8;
			answer.data[5] = canix_rtc_clock.uptime_sec;
			canix_frame_send(&answer);
			break;
		case HCAN_HMS_SYSLOG_LEVEL_SET :
#ifdef CANIX_SYSLOG
			canix_set_syslog_level(frame->data[2]);
			eeprom_write_byte((uint8_t *)EEPR_DEBUG_LEVEL, frame->data[2]);
#endif
			break;
		case HCAN_HMS_BUILD_VERSION_QUERY :
			answer.data[1] = HCAN_HMS_BUILD_VERSION_REPLAY;
			answer.data[2] = 
#include <buildver> 
				>> 8;
			answer.data[3] = 
#include <buildver>
				;
			answer.size = 4;
			canix_frame_send(&answer);
			break;
		case HCAN_HMS_CAN_EC_QUERY :
			{
				uint16_t ec = can_error_counters();
				answer.data[1] = HCAN_HMS_CAN_EC_REPLAY;
				answer.data[2] = ec >> 8;
				answer.data[3] = ec;
				answer.size = 4;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_RX_RECEIVED_QUERY :
			{
				answer.data[1] = HCAN_HMS_RX_RECEIVED_REPLAY;
				answer.data[2] = canix_rx_received >> 24;
				answer.data[3] = canix_rx_received >> 16;
				answer.data[4] = canix_rx_received >> 8;
				answer.data[5] = canix_rx_received;
				answer.size = 6;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_RX_LOST_QUERY :
			{
				answer.data[1] = HCAN_HMS_RX_LOST_REPLAY;
				answer.data[2] = canix_rx_lost >> 24;
				answer.data[3] = canix_rx_lost >> 16;
				answer.data[4] = canix_rx_lost >> 8;
				answer.data[5] = canix_rx_lost;
				answer.size = 6;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_TX_SENT_QUERY :
			{
				answer.data[1] = HCAN_HMS_TX_SENT_REPLAY;
				answer.data[2] = canix_tx_sent >> 24;
				answer.data[3] = canix_tx_sent >> 16;
				answer.data[4] = canix_tx_sent >> 8;
				answer.data[5] = canix_tx_sent;
				answer.size = 6;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_TX_DROPPED_QUERY :
			{
				answer.data[1] = HCAN_HMS_TX_DROPPED_REPLAY;
				answer.data[2] = canix_tx_dropped >> 24;
				answer.data[3] = canix_tx_dropped >> 16;
				answer.data[4] = canix_tx_dropped >> 8;
				answer.data[5] = canix_tx_dropped;
				answer.size = 6;
				canix_frame_send(&answer);
			}
			break;
		case HCAN_HMS_RX_TX_STATS_RESET : 
			{
				canix_rx_received = 0;
				canix_rx_lost = 0;
				canix_tx_sent = 0;
				canix_tx_dropped = 0;
			}
			break;
	}
}

