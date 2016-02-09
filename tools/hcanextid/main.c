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
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include <syslog.h>
#include <assert.h>

uint8_t debug = 0;
char src[32];  // Quelle
char dst[32];  // Ziel
char proto[32]; // Protokoll, z.B. SFP
char prio[32]; // Prioritaet

int parse_options(int argc, char ** argv)
{
    int opt; /* it's actually going to hold a char */

    while ((opt = getopt(argc, argv, "Dd:s:p:")) > 0) { // "x:" = expected a number
        switch (opt) {
            case 's':
                strncpy(src,optarg,sizeof(src)-1);
                fprintf(stderr,"src: %s\n",src);
                break;

            case 'd':
                strncpy(dst,optarg,sizeof(dst)-1);
                fprintf(stderr,"dst: %s\n",dst);
                break;

            case 'p':
                strncpy(proto,optarg,sizeof(proto)-1);
                fprintf(stderr,"proto: %s\n",proto);
                break;

            case 'D':
                debug = 1;
                break;

            case '?':
            case 'h':
            default:
                fprintf(stderr, "usage: %s [options]\n", basename(argv[0]));
                fprintf(stderr, "  -s  src\n");
                fprintf(stderr, "  -d  dst\n");
                fprintf(stderr, "  -p  proto\n");
                return 1;
        }
    }

    return 0;
}


static uint32_t make_id(uint16_t src, uint16_t dst, uint8_t proto, uint8_t prio) {
    return  (src & 0x3ff)         |
            ((dst & 0x3ff)  << 10) |
            ((proto & 0x07) << 20) |
            (prio << 24);
}

int main(int argc, char ** argv)
{
    // Default:
	strcpy(src, "512"); // Quelle
    strcpy(dst, "164"); // Ziel
    strcpy(proto, "1"); // SFP
    strcpy(prio, "2");  // Prioritaet

    if (parse_options(argc,argv) != 0) exit (1);

    uint32_t extid = make_id(atoi(src), atoi(dst), atoi(proto), atoi(prio));
    if(debug) printf("extid: 0x%8X\n", extid);
    return extid;
}

