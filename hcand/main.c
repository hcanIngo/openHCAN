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
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <arpa/inet.h>

#include "main.h"
#include "socket.h"

struct sockaddr_in peer_addr[MAX_PEERS];
int peer_timeout[MAX_PEERS];

int addr_cmp(const struct sockaddr_in *a, const struct sockaddr_in *b)
{
	int i;
	char *pa,*pb;
	
	pa = (char *)a;
	pb = (char *)b;

	for (i = 0; i < sizeof(struct sockaddr_in); i++)
	{
		if (pa[i] != pb[i])
			return 0;
	}

	return 1;
}

int does_peer_exist(struct sockaddr_in *peer)
{
	int i;
	for (i = 0; i < MAX_PEERS; i++)
	{
		if (addr_cmp(peer, &peer_addr[i]))
			return i;
	}
	return -1;
}

int add_peer(struct sockaddr_in *peer)
{
	int i;
	for (i = 0; i < MAX_PEERS; i++)
	{
		if (peer_addr[i].sin_addr.s_addr == 0)
		{
			peer_addr[i] = *peer;
			return i;
		}
	}
	return -1;
}

void sig_alarm(int sig)
{
	int i;

	for (i = 0; i < MAX_PEERS; i++)
	{
		// wird es benutzt ?
		if (peer_addr[i].sin_addr.s_addr)
		{
			// ja, also timeout updaten
			peer_timeout[i]++;

			// schauen, ob timeout abgelaufen ist
			if (peer_timeout[i] > TIMEOUT)
			{
				peer_timeout[i] = 0;
				peer_addr[i].sin_addr.s_addr = 0;
				//syslog(LOG_DEBUG, "hcand client timeout, cleaning up.");
			}
		}
	}	
	alarm(1);
}

int main()
{
	int sock_fd;
	size_t i;

	signal(SIGALRM, sig_alarm);
	alarm(1);

	openlog("hcand",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,
			LOG_LOCAL7);
	syslog(LOG_INFO, "hcand started.");

	sock_fd = open_server_socket(3600);

	for (i = 0; i < MAX_PEERS; i++)
	{
		peer_addr[i].sin_addr.s_addr = 0;
		peer_addr[i].sin_port = 0;
		peer_timeout[i] = TIMEOUT;
	}	

	while (1)
	{
		int i;
		int nread;
		CANFrame f;
		size_t size;
		struct sockaddr_in peer;

		bzero(&peer,sizeof(peer));
		size = sizeof(peer);
		nread = recvfrom(sock_fd, &f, sizeof(f), 0, (struct sockaddr*)&peer,
				&size);

		// 1. Schauen, ob wir von diesem schon mal was bekommen haben

		i = does_peer_exist(&peer);
		if (i != -1)
		{
			// ah, das gab es schon mal
		}
		else
		{
			i = add_peer(&peer);
			//printf("add %d, %s:%d\n", i, inet_ntoa(peer.sin_addr),
			//		ntohs(peer.sin_port));
		}

		if (i != -1)
		{
			// timeout daten updaten
			peer_timeout[i] = 0;
			
			// keine Sonderpakete zustellen:
			//
			if (f.id == 0)
				continue;

			// ueber alle peers iterieren und das Paket versenden

			for (i = 0; i < MAX_PEERS; i++)
			{
				if ((peer_addr[i].sin_addr.s_addr != 0) &&
						(addr_cmp(&peer_addr[i],&peer) == 0))
				{
					ssize_t size;
					size = sendto(sock_fd, &f, sizeof(f), 0, 
							(struct sockaddr *) &peer_addr[i],
							sizeof(peer_addr[i]));
					if (size == -1)
						syslog(LOG_ERR, "could not send packet");
				}
			}
		}
		else
		{
			syslog(LOG_ERR, "all peer slots exhaused");
		}
	}
}

