/* hcan interface to can4linux
 * based on can4linux transmit and hcanhid
 * christian at port-net . de
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <can4linux.h>

#include "hcanframe.h"

#define STDDEV "/dev/can0"
#define STDIP  "127.0.0.1"
#define STDPORT 3600

#define BUFFERSIZE 16   // must be power of two

int sock_hcand;
int can_fd;
canmsg_t CanFrame;	/* buffer for can message */
struct hcan_frame HcanFrame;
canmsg_t canBuffer[BUFFERSIZE];
hcan_Frame hcanBuffer[BUFFERSIZE];
uint8_t canBufferWPtr = 0;
uint8_t canBufferRPtr = 0;
uint8_t hcanBufferWPtr = 0;
uint8_t hcanBufferRPtr = 0;
struct timeval timeout;
uint8_t debug = 0;
char device[40];	/* string to hold the CAN device Name */
char hcand_ip[40];	/* string to hold the ip of hcand */
uint16_t hcan_port;

int parse_options(int argc, char ** argv)
{
    int opt; /* it's actually going to hold a char */

    while ((opt = getopt(argc, argv, "Dd:c:p:L")) > 0) {
        switch (opt) {
            case 'd':
                strncpy(device,optarg,sizeof(device)-1);
                fprintf(stderr,"serial device: %s\n",device);
                break;
            case 'c':
                strncpy(hcand_ip,optarg,sizeof(hcand_ip)-1);
                fprintf(stderr,"hcand ip: %s\n",hcand_ip);
                break;
            case 'p':
                sscanf(optarg, "%d", &hcan_port);
                break;
            case 'D':
                debug = 1;
                break;
            case '?':
            case 'h':
            default:
                fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
                fprintf(stderr, "  -d  can device (default: %s)\n", STDDEV);
                fprintf(stderr, "  -c  connect to hcand IP (default: %s)\n", STDIP);
                fprintf(stderr, "  -p  connect to hcand port default: %d)\n", STDPORT);
                return 1;
        }
    }

    return 0;
}

uint32_t make_id(uint16_t src, uint16_t dst, uint8_t proto, uint8_t prio) {
    uint32_t extid = (src & 0x3ff) |
        ((dst & 0x3ff) << 10) |
        ((proto & 0x07) << 20) |
        (prio << 24);
    return extid;
}

int main(int argc,char **argv)
{

    int n;
    char device[40];	/* string to hold the CAN device Name */
    fd_set rfds;		/* file descriptors for select() */
    fd_set wfds;		/* file descriptors for select() */

    strcpy(device, STDDEV);
    strcpy(hcand_ip, STDIP);
    hcan_port = STDPORT;


    // Optionen parsen:
    if (parse_options(argc,argv) != 0)
        exit (1);
    
    openlog("hcanhid",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,
                      LOG_LOCAL7);

    if( (sock_hcand = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Error while opening socket to hcand");
        exit(-1);
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_addr.s_addr = inet_addr(hcand_ip);
    sin.sin_port = htons(hcan_port);

    if(( can_fd = open(device, O_RDWR )) < 0 ) {
        fprintf(stderr,"Error opening CAN device %s\n", device);
        exit(-2);
    }
    if(debug) printf("got can fd %d\n", can_fd);

    // prepare and send can test message
    canBuffer[0].id  = make_id(35, 35, 1, 0);
    canBuffer[0].length = 2;
    canBuffer[0].flags = 0;
    canBuffer[0].data[0] = 0x11;
    canBuffer[0].data[1] = 0x22;

    n = write(can_fd, &canBuffer[0], 1);
    if(debug) printf("Wrote %d bytes to can\n", n);

    if (n != 1) {
        fprintf(stderr, "could not send complete CAN packet (%d)!", n);
        exit(-3);
    }

    // prepare and send hcan join message
    hcanBuffer[0].id  = make_id(0, 0, 0, 0);
    hcanBuffer[0].size = 0;
    n = sendto(sock_hcand, &hcanBuffer[0], sizeof(hcan_Frame), 0, (struct sockaddr *) &sin, sizeof(sin));
    if(debug) printf("Wrote %d bytes to hcand\n", n);

    if(n != sizeof(struct hcan_frame)) {
        fprintf(stderr, "could not send complete UDP packet (%d) !", n);
        exit(-4);
    }

    while(1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_SET(can_fd, &rfds);	        /* watch on fd for CAN */
        FD_SET(sock_hcand, &rfds);      /* watch on fd for UDP */
        FD_SET(can_fd, &wfds);          /* watch on fd for CAN */
        FD_SET(sock_hcand, &wfds);      /* watch on fd for UDP */
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;


	    if( select(FD_SETSIZE, &rfds, &wfds, NULL,     &timeout ) > 0 ) {
	        if( FD_ISSET(can_fd, &rfds) ) {
	            n = read(can_fd, &CanFrame, 1);
	            if (n == 1 && CanFrame.id == CANDRIVERERROR) {
	    	        if (CanFrame.flags & MSG_BUSOFF) {
		                syslog(LOG_ERR, " - BUSOFF");
	    	        } else if (CanFrame.flags & MSG_PASSIVE) {
		                syslog(LOG_ERR, " - ERROR PASSIVE");
	    	        } else {
	    	            syslog(LOG_ERR, " Error %d", CanFrame.flags);
                    }
	            } else {
                    if(debug) printf("received can frame: %lu %d\n", CanFrame.id, CanFrame.length);
                    if(((hcanBufferWPtr+1)&(BUFFERSIZE-1)) == hcanBufferRPtr) {
                        syslog(LOG_ERR, "no hcan buffer left");
                    } else {
                        hcanBuffer[hcanBufferWPtr].id = CanFrame.id;
                        hcanBuffer[hcanBufferWPtr].size = CanFrame.length;
                        memcpy(hcanBuffer[hcanBufferWPtr].data, CanFrame.data, CanFrame.length);
                        hcanBufferWPtr++;
                        hcanBufferWPtr &= BUFFERSIZE-1;
                    }
                }
	        }
	        if( FD_ISSET(sock_hcand, &rfds) ) {
                n = recv(sock_hcand, &HcanFrame, sizeof(HcanFrame), MSG_WAITALL);
                if(n != sizeof(HcanFrame)) {
                    syslog(LOG_ERR, "could not read full packet from server, dropping...");
                } else {
                    if(debug) printf("received udpframe: %lu %d\n", HcanFrame.id, HcanFrame.size);
                    if(((canBufferWPtr+1)&(BUFFERSIZE-1)) == canBufferRPtr) {
                        syslog(LOG_ERR, "no can buffer left");
                    } else {
                        canBuffer[canBufferWPtr].id = HcanFrame.id;
                        canBuffer[canBufferWPtr].length = HcanFrame.size;
                        memcpy(canBuffer[canBufferWPtr].data, HcanFrame.data, HcanFrame.size);
                        canBufferWPtr++;
                        canBufferWPtr &= BUFFERSIZE-1;
                    }
                }
	        }
	        if( FD_ISSET(can_fd, &wfds) && (canBufferWPtr != canBufferRPtr) ) {
                // send next frame to can
	            n = write(can_fd, &canBuffer[canBufferRPtr], 1);
                if(debug) printf("send can frame\n");
                if(n != 1) {
                    syslog(LOG_ERR,"could not send complete CAN packet(%d)!", n);
                }
                canBufferRPtr++;
                canBufferRPtr &= BUFFERSIZE-1;
	        }
	        if( FD_ISSET(sock_hcand, &wfds) && (hcanBufferWPtr != hcanBufferRPtr) ) {
                // send next frame to can
                n = sendto(sock_hcand, &hcanBuffer[hcanBufferRPtr], sizeof(hcan_Frame), 0, (struct sockaddr *) &sin, sizeof(sin));
                if(debug) printf("send hcan frame\n");
                if(n != sizeof(hcan_Frame)) {
                    syslog(LOG_ERR,"could not send complete hcan packet(%d)!", n);
                }
                hcanBufferRPtr++;
                hcanBufferRPtr &= BUFFERSIZE-1;
	        }
	    } /* select */
    }

    close(can_fd);
    return 0;
}
