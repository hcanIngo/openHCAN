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
 *  (c) 2006 Martin Haller, mah (at) iuse (dot) org
 *  (c) 2013 Heinz-Juergen Oertel, hj (dot) oertel (at) t-online (dot) de
 *  (c) 2014 Christian Port, christian (at) port-net (dot) de
 *  (c) 2015 Ingo Lages, i dot Lages (at) gmx (dot) de
 *           based on can4linux read/write und hcanhid
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <libgen.h>

#include <sys/time.h>
#include <time.h>


#include <fcntl.h>
#include <sys/ioctl.h>
#include <can4linux.h>

#include "frame.h"

int sock_fd; // UDP-Port zum hcand
int can4linux_fd; // fd zum can4linux-Treiber
int max_fds;
fd_set fdset;

int packet_sent_to_c4l;
int packet_recvd_from_c4l;

canmsg_t frame_to_can4linux;
int frame_to_can4linux_busy;

CANFrame frame_to_hcand;
int frame_to_hcand_busy;

char device[128];
char hcand_ip[128];

uint8_t noinit = 0;
uint8_t debug = 0;
uint8_t redDebug = 0;

int parse_options(int argc, char ** argv)
{
	int opt; /* it's actually going to hold a char */

	while ((opt = getopt(argc, argv, "d:c:nDr")) > 0) {
		switch (opt) {
			case 'd':
				strncpy(device,optarg,sizeof(device)-1);
				fprintf(stderr,"can4linux device: %s",device);
				break;
			case 'c':
				strncpy(hcand_ip,optarg,sizeof(hcand_ip)-1);
				fprintf(stderr,"hcand ip: %s",hcand_ip);
				break;
			case 'n':
				noinit = 1;
				break;
            case 'D':
                debug = 1;
                break;
            case 'r':
            	redDebug = 1;
                break;
            case '?':
			case 'h':
			default:
				fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
				fprintf(stderr, "  -d  can4linux device (default: /dev/can0)\n");
				fprintf(stderr, "  -c  connect to hcand IP (default: 127.0.0.1)\n");
				fprintf(stderr, "  -b  Debug \n");
				fprintf(stderr, "  -r  red. Debug \n");
				return 1;
		}
	}

	return 0;
}

struct timeval get_time()
{
	struct timeval this_time;
	int res;

	res = gettimeofday(&this_time,0);
	assert(res == 0);
	return this_time;
}

int time_diff_usec(struct timeval *t1, struct timeval *t2)
{
	return (t1->tv_sec * 1000000 + t1->tv_usec) -
		(t2->tv_sec * 1000000 + t2->tv_usec);
}

int main(int argc, char ** argv)
{
	int nwritten;

	struct timeval last_time,this_time;

	frame_to_hcand_busy = 0;
	frame_to_can4linux_busy = 0;

	// Default Optionen:
	strcpy(device,"/dev/can0");
	strcpy(hcand_ip,"127.0.0.1");

	// Optionen parsen:
	if (parse_options(argc,argv) != 0)
		exit (1);


	openlog("hcanhid",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,
			LOG_LOCAL7);


	sock_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(hcand_ip);
	sin.sin_port = htons(3600);

	CANFrame cf;
	cf.id = 0;
	cf.size = 0;

	nwritten = sendto(sock_fd, &cf, sizeof(CANFrame), 0,
			(struct sockaddr *) &sin, sizeof(sin));

	if (nwritten != sizeof(CANFrame))
		fprintf(stderr, "could not send complete UDP packet!");

	if(( can4linux_fd = open(device, O_RDWR )) < 0 )
	{
		fprintf(stderr, "Error opening CAN device %s\n", device);
		exit(-2);
	}
	if(debug) printf("got can4linux fd %d\n", can4linux_fd);

	syslog(LOG_INFO, "can4linuxd (hcanhid) connected to %s", device);

	if (! noinit)
	{
		syslog(LOG_ERR, "resetting can4linux-Baustein.\n");
		ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_STOP);
		ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_CLEARBUFFERS);
		ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_RESET);
		ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_START);
	}


	// Durchsatz zaehlen
	packet_sent_to_c4l = 0;
	last_time = get_time();

	while (1)
	{
		fd_set rfd;
		fd_set wfd;
		int max_fds;
		int nread;
		int res;
		int diff;
		struct timeval timeout;


		this_time = get_time();

		if ((diff = time_diff_usec(&this_time,&last_time)) > 1000000)
		{
			int sent,recvd;

			sent = packet_sent_to_c4l * 1000000 / diff;
			recvd = packet_recvd_from_c4l * 1000000 / diff;
			packet_sent_to_c4l = 0;
			packet_recvd_from_c4l = 0;
			last_time = this_time;

			if(redDebug) printf("c4l-sent:%2i,-recvd:%2i\n", sent, recvd);
		}


		timeout.tv_sec = 1;
		timeout.tv_usec = 0;


		FD_ZERO(&rfd);
		FD_ZERO(&wfd);

		max_fds = sock_fd;
		FD_SET(sock_fd, &rfd);
		FD_SET(can4linux_fd, &rfd);
		if (can4linux_fd > max_fds)
			max_fds = can4linux_fd;

		if (frame_to_can4linux_busy)
			FD_SET(can4linux_fd, &wfd);

		if (frame_to_hcand_busy)
			FD_SET(sock_fd, &wfd);

		res = select (max_fds + 1, &rfd, &wfd, NULL, &timeout);
		if (res == 0) // timeout
		{
			int n = 0;
			CANFrame cf;
			cf.id = 0;
			cf.size = 0;
			n = sendto(sock_fd, &cf, sizeof(CANFrame), 0,
					(struct sockaddr *) &sin, sizeof(sin));
		}

		// erst schreiben

		if (FD_ISSET(sock_fd, &wfd))
		{
			nwritten = sendto(sock_fd, &frame_to_hcand, sizeof(CANFrame), 0,
					(struct sockaddr *) &sin, sizeof(sin));
			if(debug) printf("wrote %d bytes to hcand\n", nwritten);
			frame_to_hcand_busy = 0;
		}

		if (FD_ISSET(can4linux_fd, &wfd))
		{
			int n = 0;
			n = write(can4linux_fd, &frame_to_can4linux, 1); // write CANFrame, 1 = ein Frame (nicht ein Byte)
			if(debug) printf("wrote %d frame to can4linux\n", n);
			if (n != 1)
			{
				fprintf(stderr, "could not send complete CAN packet (%d)!", n);
			}
			packet_sent_to_c4l++;

			frame_to_can4linux_busy = 0;
		}

		// Nur Frame vom hcand lesen, wenn
		// - der Weg zum can4Linux "frei ist"
		// - der hcand auch was zu lesen hat
		if ((!frame_to_can4linux_busy) && FD_ISSET(sock_fd, &rfd))
		{
			// read a frame from hcand
			CANFrame cf;
			nread = recv(sock_fd, &cf, sizeof(CANFrame), MSG_WAITALL);
			if (nread != sizeof(CANFrame))
			{
				syslog(LOG_ERR, "could not read full packet from server (hcand), dropping...");
				exit(1);
			}
			else
			{
				if (!frame_to_can4linux_busy)
				{
					assert(cf.size >= 0 && cf.size <= 8);
					if(debug) printf("recvd udpframe from hcand: %lu %d\n", cf.id, cf.size);
					frame_to_can4linux.id = cf.id;
					frame_to_can4linux.flags = MSG_EXT;
					frame_to_can4linux.length = cf.size;
					memcpy(frame_to_can4linux.data, cf.data, frame_to_can4linux.length);
					frame_to_can4linux_busy = 1;
				}
				else
				{
					syslog(LOG_ERR, "buffer to Can4linux overflow");
				}
			}
		}
		if (FD_ISSET(can4linux_fd, &rfd)) // can4Linux hat was zum lesen
		{
			canmsg_t cf4l;
			cf4l.flags = 0;
			int n = 0;
			n = read(can4linux_fd, &cf4l, 1); // read CANFrame, 1 = ein Frame (nicht ein Byte)
			if (n == 1 && cf4l.id == CANDRIVERERROR)
			{
				if (cf4l.flags & MSG_BUSOFF) fprintf(stderr, " - BUSOFF\n");
				else if (cf4l.flags & MSG_PASSIVE) fprintf(stderr, " - ERROR PASSIVE\n");
				else fprintf(stderr, " Error %d\n", cf4l.flags);
			}
			else
			{
				if(debug) printf("recvd frame from can4linux: %lu %d\n", (long unsigned int)cf4l.id, cf4l.length);
				if(debug) printf("id: s%d d%d p%d p%d\n", cf4l.id & 0x3ff, (cf4l.id >> 10) & 0x3ff, (cf4l.id >> 20) & 0x07, (cf4l.id >> 24)  & 0x03);
				if(debug) printf("flags: %s\n", (cf4l.flags & MSG_EXT) ? "MSG_EXT":"baseFormat");
				if(debug) printf("data(hex): %x %x %x %x\n", cf4l.data[0], cf4l.data[1], cf4l.data[2], cf4l.data[3]);
				assert(cf4l.length >= 0 && cf4l.length <= 8);

				// Einen Frame von can4linux erhalten, versenden an den hcand (per UDP)
				if (!frame_to_hcand_busy)
				{
					frame_to_hcand.id = cf4l.id;
					frame_to_hcand.size = cf4l.length;
					memcpy(frame_to_hcand.data, (cf4l.data), cf4l.length); // dest, src, size
					frame_to_hcand_busy = 1;
				}
				else
				{
					syslog(LOG_ERR, "buffer to hcand overflow");
				}

				packet_recvd_from_c4l++;
			}
		}
	}

	return 0;
}
