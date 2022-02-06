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
 *  (c) 2022 Ingo Lages, i dot Lages (at) gmx (dot) de
 */
#ifndef MQTTHCAN_H_
#define MQTTHCAN_H_

#define dStrLen 4000


#include "../include/hcan-def.h"

#include <syslog.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <linux/can.h>

bool is(const char *topic, const char *key);
void initMqttHcan(void);
int isMsg4cb(char * firstToken);
size_t prepareMsg4cb (char* topic_rx, char* payload_rx, struct can_frame * msg4cb);
size_t catHesTopic4Broker(char * strTopic, char * strPayload, const struct can_frame * cf);
uint8_t msgOfInterest4broker(const struct can_frame * cf);

#endif
