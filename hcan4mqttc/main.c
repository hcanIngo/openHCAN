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
#include "mqttclient.h"
#include <syslog.h>
#include <fcntl.h>
#include <libgen.h> // basename
#include <sys/ioctl.h> // ioctl

int can4linux_fd; // fd zum can4linux-Treiber
int max_fds;
fd_set fdset;

int packet_sent_to_c4l;
int packet_recvd_from_c4l;

canmsg_t frame_to_can4linux;
int frame_to_can4linux_busy;
struct client_info info;
int info_to_broker_busy;
char device[128];

uint8_t noinit = 0;
uint8_t debug = 0;
uint8_t redDebug = 0;

int parse_options(int argc, char ** argv)
{
	int opt; /* it's actually going to hold a char */

	while ((opt = getopt(argc, argv, "d:nDr")) > 0) {
		switch (opt) {
			case 'd':
				strncpy(device,optarg,sizeof(device)-1);
				fprintf(stderr,"can4linux device: %s",device);
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
				fprintf(stderr, "  -D  Debug \n");
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
	int res;
	struct timeval last_time,this_time;

	info_to_broker_busy = 0;
	frame_to_can4linux_busy = 0;

	strcpy(device,"/dev/can0"); // Default Option
	if (parse_options(argc,argv) != 0) exit (1); // Optionen parsen

	openlog("hcan4mqtt",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_LOCAL7);

    struct client_info info;
    memset(&info, 0, sizeof(info));
	initm(&info);

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

	packet_sent_to_c4l = 0; // Durchsatz zaehlen
	last_time = get_time();

	while (1)
	{
		fd_set rfd;
		fd_set wfd;
		int max_fds;

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

		timeout.tv_sec = 0;
		timeout.tv_usec = 0;
		FD_ZERO(&rfd);
		FD_ZERO(&wfd);
		max_fds = can4linux_fd;
		FD_SET(can4linux_fd, &rfd);
		if (frame_to_can4linux_busy) FD_SET(can4linux_fd, &wfd);
		res = select (max_fds + 1, &rfd, &wfd, NULL, &timeout);
		if (res == 0) // timeout
		{
			if(debug) printf("select timeout\n");
			publishm (&info); // info.cf bereits befuellt
			runm(&info); // TODO send to broker
		}

		// erst schreiben

		if (info_to_broker_busy)
		{
			if(debug) printf("info to broker (busy)\n");
			publishm (&info); // info.cf bereits befuellt
			runm(&info);
			info_to_broker_busy = 0;
		}

		if (FD_ISSET(can4linux_fd, &wfd)) // TODO nur wenn wir einen Frame vom broker haben
		{
			int n = 0;
			n = write(can4linux_fd, &frame_to_can4linux, 1); // write CANFrame, 1 = ein Frame (nicht ein Byte)
			if(debug) printf("wrote %d frame to can4linux\n", n);
			if (n != 1)
			{
				fprintf(stderr, "could not send complete CAN packet (%d) -> resetting can4linux-Baustein.!\n", n);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_STOP);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_CLEARBUFFERS);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_RESET);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_START);
			}
			else
				packet_sent_to_c4l++;

			frame_to_can4linux_busy = 0;
		}


		// Nur Frame vom broker lesen, wenn
		// - der Weg zum can4Linux "frei ist"
		// - der broker auch was zu lesen hat
		if ((!frame_to_can4linux_busy) && info.cf_to_read)
		{
			if (!frame_to_can4linux_busy)
			{
				assert(info.cf.size >= 0 && info.cf.size <= 8);
				// frame_to_can4linux wurde in on_message() gefuellt
				if(debug)
				{
					printf("recvd frame from broker: eid=%d %d\n", frame_to_can4linux.id, frame_to_can4linux.length);
					printf("id: s%d d%d proto%d prio%d\n",
						frame_to_can4linux.id & 0x3ff, // src
						(frame_to_can4linux.id >> 10) & 0x3ff, // dest
						(frame_to_can4linux.id >> 20) & 0x07, // prot, z.B. 1=SFP
						(frame_to_can4linux.id >> 24) & 0x03); // prio
					uint8_t i;
					for(i=0; i<frame_to_can4linux.length; i++)
						printf("d[%d] %d\n", i, frame_to_can4linux.data[i]);
				}
				else if(redDebug) printf("cb< broker\n");
				frame_to_can4linux.flags = MSG_EXT;
				frame_to_can4linux_busy = 1;
				info.cf_to_read = 0;
			}
			else
			{
				syslog(LOG_ERR, "buffer to can4linux overflow -> resetting can4linux-Baustein.\n");
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_STOP);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_CLEARBUFFERS);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_RESET);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_START);
				frame_to_can4linux_busy = 0;
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

				syslog(LOG_ERR, "resetting can4linux-Baustein.\n");
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_STOP);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_CLEARBUFFERS);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_RESET);
				ioctl(can4linux_fd, CAN_IOCTL_COMMAND, CMD_START);
			}
			else
			{
				if(debug) printf("recvd frame from can4linux");
				assert(cf4l.length >= 0 && cf4l.length <= 8);
				if(debug) printf("id=%lu, len=%d\n", (long unsigned int)cf4l.id, cf4l.length);
				if(debug) printf("id: s%d d%d p%d p%d\n", cf4l.id & 0x3ff, (cf4l.id >> 10) & 0x3ff, (cf4l.id >> 20) & 0x07, (cf4l.id >> 24)  & 0x03);
				if(debug) printf("flags: %s\n", (cf4l.flags & MSG_EXT) ? "MSG_EXT":"baseFormat");
				if(debug) printf("data(hex): %x %x %x %x\n", cf4l.data[0], cf4l.data[1], cf4l.data[2], cf4l.data[3]);

				// Einen Frame von can4linux erhalten, versenden an den broker vorbereiten
				if (!info_to_broker_busy)
				{
					info.cf.id = cf4l.id;
					info.cf.size = cf4l.length;
					memcpy(info.cf.data, (cf4l.data), cf4l.length); // dest, src, size
					info_to_broker_busy = 1;
					if(redDebug) printf("cb> broker\n");
				}
				else
				{
					syslog(LOG_ERR, "buffer to broker overflow");
				}

				packet_recvd_from_c4l++;
			}
		}

		if(redDebug) printf("runm\n");
		runm(&info);
	}

	return endm(&info);
}
