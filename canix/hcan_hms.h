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

#ifndef HCAN_HMS_H
#define HCAN_HMS_H

#ifndef CANIX_HMS
#error "CANIX_HMS muss definiert werden"
#endif

/**
 * @file	hcan_hms.h
 *
 * @author  Martin Haller
 * @date    13.3.2006
 *
 * @brief	Implementierung des HMS (HCAN Management Service)
 *
 */

/**
 * Stellt den HCAN Frame Callback dar. Es werden folgende Frames
 * beantwortet bzw. interpretiert:
 * 
 * - HCAN_HMS_PING_REQUEST
 * - HCAN_HMS_DEVICE_STATE_REQUEST
 * - HCAN_HMS_DEVICE_RESET
 * - HCAN_HMS_DEVICE_TYPE_REQUEST
 * - HCAN_HMS_INTERNAL_EEPROM_WRITE
 * - HCAN_HMS_INTERNAL_EEPROM_READ_REQUEST
 * - HCAN_HMS_UPTIME_QUERY
 * - HCAN_HMS_DEBUG_LEVEL_SET
 * - HCAN_HMS_BUILD_VERSION_QUERY
 */

void canix_SFP_HMS_handler(const canix_frame *frame);

#endif
