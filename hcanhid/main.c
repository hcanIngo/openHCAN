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


#include "unix_uart.h"
#include "frame.h"

int sock_fd;
int hi_fd;
int max_fds;
fd_set fdset;

int packet_sent;
int packet_recvd;

CANFrame frame_to_hi;
int frame_to_hi_busy;

CANFrame frame_to_hcand;
int frame_to_hcand_busy;

uint8_t ibuffer[64];
uint8_t iindex = 0;

char device[128];
char hcand_ip[128];

uint8_t noinit = 0;

int parse_options(int argc, char ** argv)
{
	int opt; /* it's actually going to hold a char */

	while ((opt = getopt(argc, argv, "d:c:N")) > 0) {
		switch (opt) {
			case 'd':
				strncpy(device,optarg,sizeof(device)-1);
				fprintf(stderr,"serial device: %s",device);
				break;
			case 'c':
				strncpy(hcand_ip,optarg,sizeof(hcand_ip)-1);
				fprintf(stderr,"hcand ip: %s",hcand_ip);
				break;
			case 'N':
				noinit = 1;
				break;
			case '?':
			case 'h':
			default:
				fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
				fprintf(stderr, "  -d  serial device (default: /dev/ttyUSB0)\n");
				fprintf(stderr, "  -c  connect to hcand IP \n");
				return 1;
		}
	}

	return 0;
}

void send_command_to_HT(int uart_fd, uint8_t command);

void reset_HI(int uart_fd)
{
	int i;

	syslog(LOG_ERR, "resetting HI...\n");

	// Jetzt so viele chars wie moeglich lesen, damit beim
	// HI v02 (USB) der Buffer im FT245 geleert wird...
	for (i = 0; i < 5; i++)
	{
		while (char_available(uart_fd))
			recchar(uart_fd);
		sleep(1);
	}

	send_command_to_HT(uart_fd, 'R');
}

void handle_serial_command(int fd, uint8_t command)
{
	switch (command)
	{
		case 'B' : 
			syslog(LOG_ERR, "sync received, reseting\n");
			iindex = 0;
			break;
		case 'Y' : // Boot reset flag Meldung
			{
				char c = recchar(fd);
				syslog(LOG_ERR, "HI has rebooted, reset flag is %c\n", c);
				iindex = 0;
				syslog(LOG_INFO, "hcanhid ready.");
			}
			break;
		case 'C' : // neues Frame kommt, also vorbereiten
			iindex = 0;
			break;
		case 'F' :
			{
				// Frame ist komplett uebertragen, versenden
				uint8_t size;
				uint32_t extended;
				int i;

				extended =
					((uint32_t)ibuffer[0]) |
					(((uint32_t)ibuffer[1]) << 8) |
					(((uint32_t)ibuffer[2]) << 16) |
					(((uint32_t)ibuffer[3]) << 24);

				if (ibuffer[4] != iindex - 5)
				{
					syslog(LOG_ERR, "Size mismatch: announced: %d, got: %d\n",
							ibuffer[4], iindex - 5);
					reset_HI(fd);
					iindex = 0;
					return;
				}
				size = ibuffer[4];


				CANFrame frame;
				frame.id = extended;
				frame.size = size;
				for (i = 0; i < size; i++)
					frame.data[i] = ibuffer[5 + i];

				if (!frame_to_hcand_busy)
				{
					frame_to_hcand = frame;
					frame_to_hcand_busy = 1;
				}
				else
				{
					syslog(LOG_ERR, "buffer to hcand overflow");
				}

				packet_recvd++;
				break;
			}
		default:
			{
				syslog(LOG_ERR,"Error: unknwon command from HI: 0x%02x\n",
						command);
				syslog(LOG_ERR, "Resetting HI...\n");
				send_command_to_HT(fd, 'R');
			}
    }
}

int firstBitsChar = -1;

uint8_t convert_hexstr2int(char msb, char lsb)
{
    uint8_t v = 0;

    if ((msb >= '0') && (msb <= '9'))
        v |= (msb - '0') << 4;
    else
        v |= ((msb - 'a') + 10) << 4;

    if ((lsb >= '0') && (lsb <= '9'))
        v |= (lsb - '0');
    else
        v |= ((lsb - 'a') + 10);

    return v;
}

char hexchars[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d',
        'e','f' };

void send_char_to_HT(int uart_fd, uint8_t c)
{
	sendchar(uart_fd, hexchars[(c & 0xf0) >> 4]);
	sendchar(uart_fd, hexchars[(c & 0x0f)]);
}

void send_command_to_HT(int uart_fd, uint8_t command)
{
	sendchar(uart_fd, command);
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

void send_frame_to_HI(int fd, const CANFrame *frame)
{
	int i;

	send_command_to_HT(fd, 'C');
	send_char_to_HT(fd, (uint32_t)frame->id & 0xff);
	send_char_to_HT(fd, ((uint32_t)frame->id >> 8) & 0xff);
	send_char_to_HT(fd, ((uint32_t)frame->id >> 16) & 0xff);
	send_char_to_HT(fd, ((uint32_t)frame->id >> 24) & 0xff);
	assert(frame->size >= 0 && frame->size <= 8);
	send_char_to_HT(fd, frame->size);
	for (i = 0; i < frame->size; i++)
		send_char_to_HT(fd, frame->data[i]);
	send_command_to_HT(fd, 'F');

	packet_sent++;
}

int main(int argc, char ** argv)
{
	int nwritten;

	struct timeval last_time,this_time;

	frame_to_hcand_busy = 0;
	frame_to_hi_busy = 0;

	// Default optionen:
	strcpy(device,"/dev/ttyUSB0");
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
		syslog(LOG_ERR,"could not send complete UDP packet !");


	hi_fd = uart_init(device);
	syslog(LOG_INFO, "hcanhid connected to %s",device);

	if (! noinit)
		reset_HI(hi_fd);

	// Durchsatz zaehlen
	packet_sent = 0;
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

			sent = packet_sent * 1000000 / diff;
			recvd = packet_recvd * 1000000 / diff;
			packet_sent = 0;
			packet_recvd = 0;
			last_time = this_time;

			//printf("sent: %2i, recvd: %2i\n",sent, recvd);
		}


		timeout.tv_sec = 1;
		timeout.tv_usec = 0;


		FD_ZERO(&rfd);
		FD_ZERO(&wfd);

		max_fds = sock_fd;
		FD_SET(sock_fd, &rfd);
		FD_SET(hi_fd, &rfd);
		if (hi_fd > max_fds)
			max_fds = hi_fd;

		if (frame_to_hi_busy)
			FD_SET(hi_fd, &wfd);

		if (frame_to_hcand_busy)
			FD_SET(sock_fd, &wfd);

		res = select (max_fds + 1, &rfd, &wfd, NULL, &timeout);
		if (res == 0) // timeout
		{
			CANFrame f;
			f.id = 0;
			f.size = 0;
			sendto(sock_fd, &f, sizeof(CANFrame), 0,
					(struct sockaddr *) &sin, sizeof(sin));
		}

		// erst schreiben

		if (FD_ISSET(sock_fd, &wfd))
		{
			nwritten = sendto(sock_fd, &frame_to_hcand, sizeof(CANFrame), 0,
					(struct sockaddr *) &sin, sizeof(sin));
			frame_to_hcand_busy = 0;
		}

		if (FD_ISSET(hi_fd, &wfd))
		{
			send_frame_to_HI(hi_fd, &frame_to_hi);
			frame_to_hi_busy = 0;
		}

		// Nur Frame vom hcand lesen, wenn
		// - der Weg zum HI "frei ist"
		// - der hcand auch was zu lesen hat
		if ((!frame_to_hi_busy) && FD_ISSET(sock_fd, &rfd))
		{
			// read a frame;
			CANFrame f;
			nread = recv(sock_fd, &f, sizeof(CANFrame), MSG_WAITALL);
			if (nread != sizeof(CANFrame))
			{
				syslog(LOG_ERR, "could not read full packet from server, dropping...");
				exit(1);
			}
			else
			{
				if (!frame_to_hi_busy)
				{
					frame_to_hi = f;
					frame_to_hi_busy = 1;
				}
				else
				{
					syslog(LOG_ERR, "buffer to HI overflow");
				}
			}
		}
		if (FD_ISSET(hi_fd, &rfd))
		{
			uint8_t c;

			c = recchar(hi_fd);

			// Commmands sind im Bereich: [A-Z]
			if ((c >= 'A') && (c <= 'Z'))
			{
				handle_serial_command(hi_fd, c);
			}
			else
			{
				// es ist ein Wert:
				if (firstBitsChar == -1)
					firstBitsChar = c;
				else
				{
					ibuffer[iindex++] = convert_hexstr2int((uint8_t)firstBitsChar,c);
					firstBitsChar = -1;
				}
			}
		}
	}

	return 0;
}
