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
#include <syslog.h>
#include <fcntl.h>
#include <libgen.h> // basename
#include <sys/ioctl.h> // ioctl

#include <linux/can.h>
#include "../hcan4mqttpc/mqttClient.h"
#include "../hcan4mqttpc/mqttHcan.h" // msgOfInterest

int sock_mqtt = INVALID_SOCKET;
int sock_can = INVALID_SOCKET;
int max_fds;
fd_set fdset;

char device[128];
char brokerHost_ip[128];

uint8_t noinit = 0;
struct can_frame canRxBuf[BUFFERSIZE];
struct can_frame mqttRxBuf[BUFFERSIZE];
uint8_t canBufWIdx = 0;
uint8_t canBufRIdx = 0;
uint8_t mqttBufWIdx = 0;
uint8_t mqttBufRIdx = 0;
uint8_t debug = 0;

int parse_options(int argc, char ** argv)
{
    int opt; /* it's actually going to hold a char */

    while ((opt = getopt(argc, argv, "Dd:c:N")) > 0)
    {
        switch (opt)
        {
            case 'd':
                strncpy(device,optarg,sizeof(device)-1);
                fprintf(stderr,"serial can device: %s\n",device);
                break;
            case 'c':
                strncpy(brokerHost_ip,optarg,sizeof(brokerHost_ip)-1);
                fprintf(stderr,"MQTT-Broker-Host-IP: %s\n",brokerHost_ip);
                break;
            case 'D':
                debug = 1;
                break;
            case '?':
            case 'h':
            default:
                fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
                fprintf(stderr, "  -d  can device (default: can0)\n");
                fprintf(stderr, "  -c  connect to MQTT-Broker-Host IP\n");
                fprintf(stderr, "  -D  aktiviere Debugausgaben\n");
                return 1;
        }
    }

    return 0;
}

int main(int argc, char ** argv)
{
	int nwritten;

	strcpy(device, "can0");
    strcpy(brokerHost_ip, "localhost"); // "192.168.1.78", "localhost", "m2m.eclipse.org"

    // Optionen parsen:
    if (parse_options(argc,argv) != 0)
    	exit (1);

    openlog("hcan4mqttpc", LOG_CONS|LOG_NDELAY|LOG_PERROR|LOG_PID, LOG_LOCAL7);

    initMqtt(brokerHost_ip); // initMqtt() -> transport_open()

    if( (sock_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
       perror("Error while opening can-socket");
       return(-2);
    }
    struct ifreq canIfr;
    struct sockaddr_can canAddr;
    struct can_frame canFrame;
    memset(&canIfr, 0x0, sizeof(canIfr));
    memset(&canAddr, 0x0, sizeof(canAddr));
    memset(&canFrame, 0x0, sizeof(canFrame));

    strcpy(canIfr.ifr_name, device);
    ioctl(sock_can, SIOCGIFINDEX, &canIfr);

    canAddr.can_family  = AF_CAN;
    canAddr.can_ifindex = canIfr.ifr_ifindex;
    TRACE("%s at index %d\n", device, canIfr.ifr_ifindex);
    if(bind(sock_can, (struct sockaddr *)&canAddr, sizeof(canAddr)) < 0)
    {
        perror("Error in socket bind");
        return(-3);
    }
    /* "RQ": Nachricht "HES_DEVICE_STATES_REQUEST" an den CAN-Bus

       Mit Empfang von "HCAN_HES_DEVICE_STATES_REQUEST"
       senden alle C1612-Controller alle ihre konfigurierten
       Lampen-, Sonstige-, Rolladen-, Heizungs- Zustaende. */
    char msg[3];
    strncpy(msg, "RQ", sizeof msg);
    createMsg4cb(msg);

    fd_set recv_fdset;
    fd_set send_fdset;
    int max_fd;
    int nread;
    struct timeval timeout;
    max_fd = sock_mqtt;
    if(sock_can > max_fd)
        max_fd = sock_can;

    //publishMqttMsg("cb>", (unsigned char*)"START-2CB", strlen("START-2CB")); // topic "cb>" (vom CAN-Bus)

    while (1)
    {
        int rtn;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        FD_ZERO(&recv_fdset);
        FD_ZERO(&send_fdset);

        FD_SET(sock_can, &recv_fdset);
        FD_SET(sock_mqtt, &recv_fdset);

        if(canBufWIdx != canBufRIdx) FD_SET(sock_can, &send_fdset);
        if(mqttBufWIdx != mqttBufRIdx) FD_SET(sock_mqtt, &send_fdset);

        rtn = select (max_fd+1, &recv_fdset, &send_fdset, NULL, &timeout);
        if(rtn > 0) // no timeout
        {
            if (FD_ISSET(sock_can, &recv_fdset))
            {
                // read a frame from can:
            	memset(&canFrame, 0, sizeof(struct can_frame)); // WICHTIG!
            	nread = recv(sock_can, &canFrame, sizeof(struct can_frame), MSG_WAITALL);
                if (nread != sizeof(struct can_frame)) {
                    syslog(LOG_ERR, "could not read full packet from can, dropping...\n");
                    exit(1);
                }
                else
                {
                    if(((canBufWIdx+1)&(BUFFERSIZE-1)) == canBufRIdx)
                    {
                        TRACE("no rx-can-buffer left\n");
                    }
                    else if (msgOfInterest4broker(&canFrame))
					{
                    	canRxBuf[canBufWIdx].can_id = canFrame.can_id & CAN_EFF_MASK;
						canRxBuf[canBufWIdx].can_dlc = canFrame.can_dlc;
						memcpy(canRxBuf[canBufWIdx].data, canFrame.data, canFrame.can_dlc);

						canBufWIdx++;
						canBufWIdx &= BUFFERSIZE-1;
					}
                    else
                    {
                    	//TRACE("not a msg for broker\n");
                    }
                }
            }

        	if (FD_ISSET(sock_mqtt, &recv_fdset))
            {
        		if(((mqttBufWIdx+1)&(BUFFERSIZE-1)) == mqttBufRIdx)
				{
					 TRACE("no rx-mqtt-buffer left\n");
				}
        		else
        		{
        			recvMqttMsg(); // ggf. mqttBufWIdx++
        		}
            }

			if ((canBufWIdx != canBufRIdx) && FD_ISSET(sock_can, &send_fdset)) // something from 2send:  cb --> mqtt-broker?
			{
				char str[200];
				memset(str, '\0', sizeof(str)); // WICHTIG!
				int strLen = catHesTopic4Broker(str, &canRxBuf[canBufRIdx]);
				if (0 < strLen)
				{
					TRACE("cb> mqtt %s\n", str);
					publishMqttMsg("cb>", (unsigned char*)str, strLen); // topic "cb>" (vom CAN-Bus)
				}

				canBufRIdx++;
				canBufRIdx &= BUFFERSIZE-1;
			}

			if ((mqttBufWIdx != mqttBufRIdx) && FD_ISSET(sock_mqtt, &send_fdset)) // something from 2send:  mqtt-broker --->  cb?
			{
				nwritten = write(sock_can, &mqttRxBuf[mqttBufRIdx], sizeof(struct can_frame));
				if (nwritten == sizeof(struct can_frame))
				{
					if(debug)
					{
						// TEST-Ausgabe:
						char str[200];
						memset(str, '\0', sizeof(str)); // WICHTIG!
						snprintf(str, 16, ".%x", (uint16_t)mqttRxBuf[mqttBufRIdx].can_id);
						int i;
						for (i=0; i<mqttRxBuf[mqttBufRIdx].can_dlc; i++)
						{
							snprintf(&str[strlen(str)], 16, ", %u", mqttRxBuf[mqttBufRIdx].data[i]);
						}
						TRACE("cb<--mqtt %s\n", str);
					}

					mqttBufRIdx++;
					mqttBufRIdx &= BUFFERSIZE-1;
				}
				else
				{
					syslog(LOG_ERR,"could not send complete CAN packet: written=%d, data-size=%d!\n", nwritten, mqttRxBuf[mqttBufRIdx].can_dlc);
				}
        	}
        }
    }


    return 0;
}

