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

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

#include "unix_uart.h"

int uart_init(const char *device)
{
	struct termios settings;
	int uart_fd;

	uart_fd = open(device,O_RDWR | O_SYNC);
	if (uart_fd == -1)
	{
		syslog(LOG_CRIT,"could not open %s: %s\n",device,strerror(errno));
		exit(1);
	}

	tcgetattr(uart_fd,&settings);
	settings.c_lflag &= ~ICANON;
	settings.c_lflag &= ~ECHO;

	cfsetispeed(&settings, B38400);
	cfsetospeed(&settings, B38400);

	if (tcsetattr(uart_fd,TCSANOW,&settings) != 0)
	{
		syslog(LOG_CRIT,"could not set attributes\n");
		exit (1);
	}

	return uart_fd;
}

char recchar(int uart_fd)
{
	char c;
	int nread;
	nread = read(uart_fd,&c,1);
	assert(nread == 1);

#ifdef DEBUG0
	syslog(LOG_CRIT,"%c",c);
	fflush(stderr);
#endif
	return c;
}

void sendchar(int uart_fd, char c)
{
	write(uart_fd,&c,1);
}

int char_available(int uart_fd)
{
	int nread;

	if (ioctl(uart_fd,FIONREAD,&nread) == 0)
		return nread;
	else
		return 0;
}
