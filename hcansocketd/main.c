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
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include <syslog.h>
#include <assert.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "../include/hcanframe.h"

#define BUFFERSIZE 16 // must be power of two
#define INVALID_SOCKET -1

int sock_hcan = INVALID_SOCKET;
int sock_can = INVALID_SOCKET;

char device[128];
char hcand_ip[128];

struct can_frame canTxBuf[BUFFERSIZE];
CANFrame hcanTxBuf[BUFFERSIZE];
uint8_t canBufWIdx = 0;
uint8_t canBufRIdx = 0;
uint8_t hcanBufWIdx = 0;
uint8_t hcanBufRIdx = 0;

uint8_t debug = 0;


static int parse_options(int argc, char ** argv)
{
    int opt; /* it's actually going to hold a char */

    while ((opt = getopt(argc, argv, "Dd:c:N")) > 0) {
        switch (opt) {
            case 'd':
                strncpy(device,optarg,sizeof(device)-1);
                fprintf(stderr,"serial can device: %s\n",device);
                break;

            case 'c':
                strncpy(hcand_ip,optarg,sizeof(hcand_ip)-1);
                fprintf(stderr,"hcand ip: %s\n",hcand_ip);
                break;

            case 'D':
                debug = 1;
                break;

            case '?':
            case 'h':
            default:
                fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
                fprintf(stderr, "  -d  can device (default: can0)\n");
                fprintf(stderr, "  -c  connect to hcand IP \n");
                fprintf(stderr, "  -D  aktiviere Debugausgaben\n");
                return 1;
        }
    }

    return 0;
}

static uint32_t make_id(uint16_t src, uint16_t dst, uint8_t proto, uint8_t prio)
{
    uint32_t extid = (src & 0x3ff) |
                ((dst & 0x3ff) << 10) |
                ((proto & 0x07) << 20) | 
                (prio << 24);
    return extid;
}

static void initHcan(void)
{
	if ((sock_hcan = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		perror("error while opening hcan socket");
		exit(-1);
	}

	hcanTxBuf[hcanBufWIdx].id  = make_id(0, 0, 0, 0);
    hcanTxBuf[hcanBufWIdx].size = 0;

    hcanBufWIdx++; // "wir haben was eingelesen" -> tx
    hcanBufWIdx &= BUFFERSIZE-1;
}

static void initSocketcan(struct sockaddr_in * sin)
{
    memset(&sin, 0, sizeof(sin));
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = inet_addr(hcand_ip);
    sin->sin_port = htons(3600);

    if ((sock_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
       perror("error while opening can socket\n");
       exit(-2);
    }

    struct ifreq can_ifr;
    strcpy(can_ifr.ifr_name, device);
    ioctl(sock_can, SIOCGIFINDEX, &can_ifr);

    struct sockaddr_can can_addr;
    can_addr.can_family  = AF_CAN;
    can_addr.can_ifindex = can_ifr.ifr_ifindex;
    if (bind(sock_can, (struct sockaddr *)&can_addr, sizeof(can_addr)) < 0)
    {
        perror("error in socket bind\n");
        exit(-3);
    }

    if(debug) printf("%s at index %d\n", device, can_ifr.ifr_ifindex);
    canTxBuf[canBufWIdx].can_id  = make_id(35, 35, 1, 0) | CAN_EFF_FLAG;
    canTxBuf[canBufWIdx].can_dlc = 2;
    canTxBuf[canBufWIdx].data[0] = 0x11;
    canTxBuf[canBufWIdx].data[1] = 0x22;

    canBufWIdx++; // "wir haben was eingelesen" -> tx
    canBufWIdx &= BUFFERSIZE-1;
}

// write to canTxBuf
static int rxFromHcan(void)
{
	CANFrame hcanFrame;
	memset(&hcanFrame, 0, sizeof(hcanFrame));
	int nread = recv(sock_hcan, &hcanFrame, sizeof(hcanFrame), MSG_WAITALL); // read a frame from hcan
	if (nread == sizeof(hcanFrame))
    {
    	if(debug) printf("received udpframe: %u %d\n", hcanFrame.id, hcanFrame.size);

        if(((canBufWIdx+1)&(BUFFERSIZE-1)) == canBufRIdx)
            printf("no can buffer left\n");

        else
        {
            canTxBuf[canBufWIdx].can_id = hcanFrame.id | CAN_EFF_FLAG;
            canTxBuf[canBufWIdx].can_dlc = hcanFrame.size;
            memcpy(canTxBuf[canBufWIdx].data, hcanFrame.data, hcanFrame.size);
            canBufWIdx++;
            canBufWIdx &= BUFFERSIZE-1;
        }
    }
    else if (nread == -1)
    	return -1;

    else
    {
        syslog(LOG_ERR, "could not read full packet from hcand, dropping...\n");
        exit(1);
    }

	return -1;
}

// write to hcanTxBuf
static int rxFromCb(void)
{
	struct can_frame canFrame;
	memset(&canFrame, 0, sizeof(canFrame));
	int nread = recv(sock_can, &canFrame, sizeof(struct can_frame), MSG_WAITALL); // read a frame from can
    if (nread == sizeof(struct can_frame))
    {
        if(debug) printf("received can frame: %u %d\n", canFrame.can_id, canFrame.can_dlc);

        if(((hcanBufWIdx+1)&(BUFFERSIZE-1)) == hcanBufRIdx)
        	printf("no hcan buffer left\n");

        else
        {
            hcanTxBuf[hcanBufWIdx].id = canFrame.can_id & CAN_EFF_MASK;
            hcanTxBuf[hcanBufWIdx].size = canFrame.can_dlc;
            memcpy(hcanTxBuf[hcanBufWIdx].data, canFrame.data, canFrame.can_dlc);
            hcanBufWIdx++;
            hcanBufWIdx &= BUFFERSIZE-1;
        }
    }
    else if (nread == -1)
    	return -1;

    else
    {
    	syslog(LOG_ERR, "could not read full packet from can, dropping...\n");
        exit(1);
    }

    return -1;
}

// read from canTxBuf
static void tx2hcan(void)
{
	if(debug) printf("tx2hcan - send can frame\n");

	int nwritten = write(sock_can, &canTxBuf[canBufRIdx], sizeof(struct can_frame));

	if (nwritten != sizeof(struct can_frame))
        syslog(LOG_ERR,"could not send complete CAN packet !");
}

// read from hcanTxBuf
static void tx2cb(struct sockaddr_in sin)
{
	if(debug) printf("tx2cb - send hcan frame\n");

	int nwritten = sendto(sock_hcan, &hcanTxBuf[hcanBufRIdx], sizeof(CANFrame), 0,
                (struct sockaddr *) &sin, sizeof(sin));

	if (nwritten != sizeof(CANFrame))
        syslog(LOG_ERR,"could not send complete UDP packet !");
}

int main(int argc, char ** argv)
{
    strcpy(hcand_ip, "127.0.0.1");
    strcpy(device, "can0");

    // Optionen parsen:
    if (parse_options(argc,argv) != 0)
        exit (1);

    openlog("hcansocketd",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,
            LOG_LOCAL7);

    initHcan();

    struct sockaddr_in sin;
    initSocketcan(&sin); // hcand_ip, device

    fd_set recv_fdset;
    fd_set send_fdset;

    struct timeval timeout;
    int max_fd = sock_hcan;
    if(sock_can > max_fd) max_fd = sock_can;

    while (1)
    {
        timeout.tv_sec = 2;
        timeout.tv_usec = 500000;
        FD_ZERO(&recv_fdset);
        FD_ZERO(&send_fdset);

        FD_SET(sock_hcan, &recv_fdset);
        FD_SET(sock_can, &recv_fdset);
        if(hcanBufWIdx != hcanBufRIdx) FD_SET(sock_hcan, &send_fdset);
        if(canBufWIdx != canBufRIdx) FD_SET(sock_can, &send_fdset);

        int ret = select (max_fd + 1, &recv_fdset, &send_fdset, NULL, &timeout);
        if (ret > 0) // no timeout?
        {
            if (FD_ISSET(sock_hcan, &recv_fdset))
            {
            	int rc = rxFromHcan();
    			if (rc == -1)
    			{
    				shutdown(sock_hcan, SHUT_WR);
    				recv(sock_hcan, NULL, (size_t)0, 0);
    				close(sock_hcan);
    				initHcan(); // ...keepAliveInterval
    			}
            }

            if (FD_ISSET(sock_can, &recv_fdset))
            {
            	rxFromCb();
            }

            if (canBufWIdx != canBufRIdx)
            {
            	tx2hcan();
                canBufRIdx++;
                canBufRIdx &= BUFFERSIZE-1;
            }

            if (hcanBufWIdx != hcanBufRIdx)
            {
            	tx2cb(sin);
                hcanBufRIdx++;
                hcanBufRIdx &= BUFFERSIZE-1;
            }
        }
    }

    return 0;
}

