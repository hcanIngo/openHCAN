#include <libhcan++/traceable_error.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <driver_factory.h>
#include <iostream>

#ifndef __WIN32__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif
#include <errno.h>

using namespace hcan;
using namespace std;

board_error::board_error (const string &s) :
	traceable_error(s)
{
}

board_connection::board_connection (transport_connection &tcon,
		uint16_t src, uint16_t dst, bool ignore_type, uint8_t arch) :
	m_tcon(tcon),
	m_driver(0),
	m_src(src),
	m_dst(dst)
{
	uint8_t type;
	type = 255;

	m_tcon.send_PING_REQUEST(m_src, m_dst);
	m_tcon.recv_PING_REPLAY(m_dst, m_src);

	if (!ignore_type && (arch == hcan::HCAN_ARCH_AUTODETECT))
	{
		m_tcon.send_DEVICE_TYPE_REQUEST(m_src, m_dst);
		m_tcon.recv_DEVICE_TYPE_REPLAY(m_dst, m_src, &arch, &type);
	}

	hcan::driver_factory factory;
	m_driver = factory.make_driver(*this, arch, type);
}

board_connection::~board_connection() throw ()
{
}

