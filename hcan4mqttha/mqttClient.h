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
#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include <time.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <err.h>
#include <linux/can.h>

#define INVALID_SOCKET -1
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>

extern int sock_mqtt;
#define BUFFERSIZE 16 // must be power of two
extern struct can_frame mqttRxBuf[BUFFERSIZE];
extern uint8_t mqttBufWIdx;
extern uint8_t mqttBufRIdx;
extern uint8_t debug;
extern uint8_t debugWay;
extern bool HaOnline;
extern bool HaOnlineStateChanged;


#ifdef DEBUG
#define TRACE(...) do { if(debug) printf(__VA_ARGS__); } while (0)
#define RETURN(...) do { if(debug) printf(__VA_ARGS__); return;} while (0)
#else
#define TRACE(...)
#define RETURN(...)
#endif

void initMqtt(char *brokerHost_ip);
void createReqMsg4cb(char* msg);
int publishMqttMsg(char* pubTopic, const unsigned char* payload, const int payloadlen);
int rxFromBroker(void);
void endMqtt(void);

#endif
