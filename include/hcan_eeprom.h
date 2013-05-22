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

#ifndef HCAN_EEPROM_H
#define HCAN_EEPROM_H

/*
 *   Internal EEPROM - Wo liegt was ?
 *   --------------------------------
 *
 *   0    - Ptr auf den App/Firmware EEPROM Bereich, normalerweise 32;
 *          wird z.Z. nicht verwendet, d.h. nicht interpretiert; die Constante
 *          ist in eds.h definiert
 *   1    - Reserviert
 *   2    - HCAN Adresse lo
 *   3    - HCAN Adresse hi
 *   4    - Type ID (siehe ./geraete-einteilungen)
 *   5    - Debug Level
 *   
 *   16:17 - CRC16 Checksum Flash
 *   18:19 - Groesse der Firmware in Bytes
 *
 *   Eventuell geplant:
 *   20:21 - Unix Timestamp des letzten Firmware-Schreibens
 *   22:23 - 16Bit Firmware Version Format: 4bit.4bit.8bit
 *   24:25 - CRC16 ueber 16:23
 *
 *   32 - ... Application Data
 *
 */

#define EEPR_APP_START                 0
#define EEPR_SELF_ADDR                 2
#define EEPR_SELF_ADDR_HI              3
#define EEPR_BOARD_TYPE                4
#define EEPR_DEBUG_LEVEL               5
//#define EEPR_DS_TOPNODE_ID             6

//   16:17 - CRC16 Checksum Flash
#define EEPR_FIRMWARE_CRC16           16
//   18:19 - Groesse der Firmware in Bytes
#define EEPR_FIRMWARE_SIZE            18

#endif
