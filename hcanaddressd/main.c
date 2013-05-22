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


typedef struct
{
	int socket;
	int address;
} peer_t;

peer_t peers[MAX_PEER];

void init_peer_array()
{
	// Adressen eintragen und Sockets
	// mit -1 initialisieren
	
	int i;
	for (i = 0; i < MAX_PEER; i++)
	{
		peers[i].socket = -1;
		peers[i].address = 512 + i;
	}
}

int assign_address(int sock)
{
	int i;
	for (i = 0; i < MAX_PEER; i++)
	{
		if (peers[i].socket == -1)
		{
			peers[i].socket = sock;
			return peers[i].address;
		}
	}
	return -1;
}

int main()
{
	int sock_fd;

	openlog("hcanaddressd",LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID,
			LOG_LOCAL7);
	syslog(LOG_INFO, "hcanaddressd started.");

	sock_fd = open_server_socket(3601);

	init_peer_array();

	while (1)
	{
		//syslog(LOG_ERR, "all peer slots exhaused");

		fd_set rfd;
		int max_fds;
		int res;
		int i;
		struct timeval timeout;

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;


		FD_ZERO(&rfd);

		max_fds = sock_fd;
		FD_SET(sock_fd, &rfd);
		for (i = 0; i < MAX_PEER; i++)
		{
			if (peers[i].socket != -1)
				FD_SET(peers[i].socket, &rfd);
			if (peers[i].socket > max_fds)
				max_fds = peers[i].socket;
		}	
		res = select (max_fds + 1, &rfd, 0, 0, &timeout);

		if (res)
		{
			if (FD_ISSET(sock_fd, &rfd))
			{
				int peer_sock;
				int32_t new_address;
				peer_sock = accept(sock_fd,0,0);
				if (peer_sock != -1)
				{
					new_address = assign_address(peer_sock);
					if (new_address == -1)
					{
						close(peer_sock);
					}
					else
					{
						syslog(LOG_INFO,"assigned %d...\n",new_address);
						write(peer_sock,&new_address,4);
					}
				}
			}

			// Ueber alle Peers interieren...
			for (i = 0; i < MAX_PEER; i++)
			{
				// Falls ein Peer verwendet wird, d.h. der Socket nicht
				// -1 ist und select auf diesem Socket etwas zu lesen
				// hat:
				if ((peers[i].socket != -1) && 
						(FD_ISSET(peers[i].socket,&rfd)))
				{
					char dummy[64];
					size_t nread;
					nread = read(peers[i].socket,dummy,sizeof(dummy));

					// Falls der Client die Verbindung geschlossen hat:
					if (nread == 0)
					{
						// ... seine Adresse wieder freigeben
						syslog(LOG_INFO,"released %d.\n",peers[i].address);
						close(peers[i].socket);
						peers[i].socket = -1;
					}
				}
			}
		}
	}
}

