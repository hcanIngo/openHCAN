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
#include "hcanframe.h"

#define BUFFERSIZE 16 // must be power of two

int sock_inet;
int sock_can;
int max_fds;
fd_set fdset;

int packet_sent;
int packet_recvd;

char device[128];
char hcand_ip[128];

uint8_t noinit = 0;
struct can_frame canBuffer[BUFFERSIZE];
struct hcan_frame hcanBuffer[BUFFERSIZE];
uint8_t canBufferWPtr = 0;
uint8_t canBufferRPtr = 0;
uint8_t hcanBufferWPtr = 0;
uint8_t hcanBufferRPtr = 0;
struct can_frame CanFrame;
struct hcan_frame HcanFrame;
uint8_t debug = 0;


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

int parse_options(int argc, char ** argv)
{
    int opt; /* it's actually going to hold a char */

    while ((opt = getopt(argc, argv, "Dd:c:N")) > 0) {
        switch (opt) {
            case 'd':
                strncpy(device,optarg,sizeof(device)-1);
                fprintf(stderr,"serial device: %s\n",device);
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

int main(int argc, char ** argv)
{
    int nwritten;

    strcpy(hcand_ip, "127.0.0.1");
    strcpy(device, "can0");

    // Optionen parsen:
    if (parse_options(argc,argv) != 0)
        exit (1);

    openlog("hcanhid",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,
            LOG_LOCAL7);

    if( (sock_inet = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("Error while opening socket");
        return(-1);
    }
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(hcand_ip);
    sin.sin_port = htons(3600);

    if( (sock_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Error while opening socket");
       return(-2);
    }
    struct sockaddr_can can_addr;
    struct can_frame CanFrame;
    struct ifreq can_ifr;

    strcpy(can_ifr.ifr_name, device);
    ioctl(sock_can, SIOCGIFINDEX, &can_ifr);

    can_addr.can_family  = AF_CAN;
    can_addr.can_ifindex = can_ifr.ifr_ifindex;
    if(debug) printf("%s at index %d\n", device, can_ifr.ifr_ifindex);
    if(bind(sock_can, (struct sockaddr *)&can_addr, sizeof(can_addr)) < 0) {
        perror("Error in socket bind");
        return(-3);
    }
    canBuffer[0].can_id  = make_id(35, 35, 1, 0) | CAN_EFF_FLAG;
    canBuffer[0].can_dlc = 2;
    canBuffer[0].data[0] = 0x11;
    canBuffer[0].data[1] = 0x22;
 
    nwritten = write(sock_can, &canBuffer[0], sizeof(struct can_frame));
 
    if(debug) printf("Wrote %d bytes\n", nwritten);
    if (nwritten != sizeof(struct can_frame)) {
        syslog(LOG_ERR,"could not send complete CAN packet !");
        exit(-4);
    }

 
    hcanBuffer[0].id  = make_id(0, 0, 0, 0);
    hcanBuffer[0].size = 0;
    nwritten = sendto(sock_inet, &hcanBuffer[0], sizeof(struct hcan_frame), 0,
            (struct sockaddr *) &sin, sizeof(sin));

    if(debug) printf("Wrote %d bytes\n", nwritten);
    if (nwritten != sizeof(struct hcan_frame)) {
        syslog(LOG_ERR,"could not send complete UDP packet !");
        exit(-5);
    }


    // Durchsatz zaehlen
    packet_sent = 0;

    fd_set recv_fdset;
    fd_set send_fdset;
    int max_fd;
    int nread;
    struct timeval timeout;
    max_fd = sock_inet;
    if(sock_can > max_fd)
        max_fd = sock_can;

    while (1) {
        int ret;
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;
        FD_ZERO(&recv_fdset);
        FD_ZERO(&send_fdset);
        FD_SET(sock_inet, &recv_fdset);
        FD_SET(sock_can, &recv_fdset);
        if(hcanBufferWPtr != hcanBufferRPtr)
        	FD_SET(sock_inet, &send_fdset);
        if(canBufferWPtr != canBufferRPtr)
        	FD_SET(sock_can, &send_fdset);
        ret = select (max_fd + 1, &recv_fdset, &send_fdset, NULL, &timeout);
        if(ret > 0) {// no timeout
            if (FD_ISSET(sock_inet, &recv_fdset)) {
                // read a frame from inet;
                nread = recv(sock_inet, &HcanFrame, sizeof(HcanFrame), MSG_WAITALL);
                if (nread != sizeof(HcanFrame)) {
                    syslog(LOG_ERR, "could not read full packet from server, dropping...");
                    exit(1);
                }
                else {
                    if(debug) printf("received udpframe: %u %d\n", HcanFrame.id, HcanFrame.size);
                    if(((canBufferWPtr+1)&(BUFFERSIZE-1)) == canBufferRPtr) {
                        printf("no buffer left\n");
                    } else {
                        canBuffer[canBufferWPtr].can_id = HcanFrame.id | CAN_EFF_FLAG;
                        canBuffer[canBufferWPtr].can_dlc = HcanFrame.size;
                        memcpy(canBuffer[canBufferWPtr].data, HcanFrame.data, HcanFrame.size);
                        canBufferWPtr++;
                        canBufferWPtr &= BUFFERSIZE-1;
                    }
                }
            }
            if (FD_ISSET(sock_can, &recv_fdset)) {
                // read a frame from can;
                nread = recv(sock_can, &CanFrame, sizeof(struct can_frame), MSG_WAITALL);
                if (nread != sizeof(struct can_frame)) {
                    syslog(LOG_ERR, "could not read full packet from can, dropping...");
                    exit(1);
                }
                else {
                    if(debug) printf("received can frame: %u %d\n", CanFrame.can_id, CanFrame.can_dlc);
                    if(((hcanBufferWPtr+1)&(BUFFERSIZE-1)) == hcanBufferRPtr) {
                        printf("no buffer left\n");
                    } else {
                        hcanBuffer[hcanBufferWPtr].id = CanFrame.can_id & CAN_EFF_MASK;
                        hcanBuffer[hcanBufferWPtr].size = CanFrame.can_dlc;
                        memcpy(hcanBuffer[hcanBufferWPtr].data, CanFrame.data, CanFrame.can_dlc);
                        hcanBufferWPtr++;
                        hcanBufferWPtr &= BUFFERSIZE-1;
                    }
                }
            }
            if ((canBufferWPtr != canBufferRPtr) && FD_ISSET(sock_can, &send_fdset)) {
                nwritten = write(sock_can, &canBuffer[canBufferRPtr], sizeof(struct can_frame));
                if(debug) printf("send can frame\n");
                if (nwritten != sizeof(struct can_frame))
                    syslog(LOG_ERR,"could not send complete CAN packet !");
                canBufferRPtr++;
                canBufferRPtr &= BUFFERSIZE-1;
            }
            if ((hcanBufferWPtr != hcanBufferRPtr) && FD_ISSET(sock_inet, &send_fdset)) {
                nwritten = sendto(sock_inet, &hcanBuffer[hcanBufferRPtr], sizeof(struct hcan_frame), 0,
                            (struct sockaddr *) &sin, sizeof(sin));
                if(debug) printf("send hcan frame\n");
                if (nwritten != sizeof(struct hcan_frame))
                    syslog(LOG_ERR,"could not send complete UDP packet !");
                hcanBufferRPtr++;
                hcanBufferRPtr &= BUFFERSIZE-1;
            }
        }
    }

    return 0;
}

