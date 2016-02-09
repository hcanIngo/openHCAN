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
 *  (c) 2016 Ingo Lages, i dot Lages (at) gmx (dot) de
 */
#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <err.h>
#include "mosquitto.h"
#include <can4linux.h>

extern canmsg_t frame_to_can4linux;
extern uint8_t debug;

#ifdef DEBUG
#define TRACE(...) do { if(debug) printf(__VA_ARGS__); } while (0)
#define RETURN(...) do { if(debug) printf(__VA_ARGS__); return;} while (0)
#else
#define TRACE(...)
#define RETURN(...)
#endif

#define KEEPALIVE_SECONDS 60 // Intervall der keep-alive-messages vom MQTT-Broker
#define BROKER_HOSTNAME "localhost"
#define BROKER_PORT 1883

typedef struct
{
	uint32_t id;
	uint32_t  size;
	uint8_t  data[8];
} CANFrame;

struct client_info {
    struct mosquitto *m;
    CANFrame cf;
    uint8_t cf_to_read;
};

void exitm(const char *msg);
void initm(struct client_info *info);
bool set_callbacksm(struct mosquitto *m);
bool connectm(struct mosquitto *m);
void runm(struct client_info *info);
int endm(struct client_info *info);
void publishm(struct client_info *info);

#endif
