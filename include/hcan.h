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

#ifndef HCAN_H
#define HCAN_H

// Dieser Header deklariert saemliche Konstanten zum HCAN Protokoll, die
// in ../doc/* im Detail erlaeutert sind
//

/////////////////////////////////////////////////////////////////////
// Multicast Adressen

#include <hcan_multicast.h>

/////////////////////////////////////////////////////////////////////
// Protokolle (3Bits im extid)

#define HCAN_PROTO_SFP                                       1
#define HCAN_PROTO_DAAP                                      2
#define HCAN_PROTO_SYSLOG                                    3

/////////////////////////////////////////////////////////////////////
// Services

#define HCAN_SRV_HMS                                         1
#define HCAN_SRV_OWS                                         2
#define HCAN_SRV_RCS                                         3
#define HCAN_SRV_SLS                                         4
#define HCAN_SRV_HES                                         5
#define HCAN_SRV_RTS                                         6
#define HCAN_SRV_EDS                                         7
#define HCAN_SRV_USVS                                        8
#define HCAN_SRV_EBUS                                        9
#define HCAN_SRV_WS                                         10
#define HCAN_SRV_OAPS                                       11

// saemtliche Konstanten, aus der DB generiert:
#include "hcan-def.h"

// HMS Commands
// Details siehe ../doc/HMS

#define HCAN_HMS_DEV_STATE_BOOTING                           0
#define HCAN_HMS_DEV_STATE_BOOTLOADER                        1
#define HCAN_HMS_DEV_STATE_APPLICATION                       2

// DEPRECATED : OWS Commands

#define HCAN_OWS_PORTS_QUERY                                 1
#define HCAN_OWS_PORTS_REPLAY                                2
#define HCAN_OWS_DS1820_QUERY                                3
#define HCAN_OWS_DS1820_REPLAY                               4
#define HCAN_OWS_DS1820_READ_TEMP_REQUEST                    5
#define HCAN_OWS_DS1820_READ_TEMP_REPLAY                     6
#define HCAN_OWS_ERROR                                     255

// DEPRECATED : RCS (relais control service), siehe ../doc/RCS

#define HCAN_RCS_PORTS_QUERY                                 1
#define HCAN_RCS_PORTS_REPLAY                                2
#define HCAN_RCS_RELAIS_TYPE_QUERY                           3
#define HCAN_RCS_RELAIS_TYPE_REPLAY                          4
#define HCAN_RCS_SET_RELAIS_TYPE                             5
#define HCAN_RCS_RELAIS_GROUP_QUERY                          6
#define HCAN_RCS_RELAIS_GROUP_REPLAY                         7
#define HCAN_RCS_SET_RELAY_GROUP                             8
#define HCAN_RCS_RELAIS_STATE_QUERY                         10
#define HCAN_RCS_RELAIS_STATE_REPLAY                        11
#define HCAN_RCS_RELAIS_ON                                  20
#define HCAN_RCS_RELAIS_OFF                                 21
#define HCAN_RCS_RELAIS_TOGGLE                              22
#define HCAN_RCS_MULTICAST_GROUP_ON                         23
#define HCAN_RCS_MULTICAST_GROUP_OFF                        24
#define HCAN_RCS_MULTICAST_GROUP_TOGGLE                     25
#define HCAN_RCS_EEPROM_SAVE                                30
#define HCAN_RCS_ERROR                                     255

#endif

