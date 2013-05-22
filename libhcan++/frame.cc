#include <frame.h>
#include <iostream>
#include <string.h>

#ifndef __WIN32__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#else

#include <winsock2.h>

#endif

#include <unistd.h>
#include <assert.h>

#include <libhcan++/frame_message_description.h>

using namespace hcan;
using namespace std;

bool frame::polite_mode = true;
int frame::polite_wait_time = 50000;

io_error::io_error (const string &s) :
	traceable_error(s)
{
}

frame::frame() :
	m_src(0),
	m_dst(0),
	m_proto(0),
	m_prio(0),
	m_size(0)
{
}

frame::frame(const uint16_t src, const uint16_t dst, 
		const uint8_t proto, const uint8_t data[], const uint32_t size):
	m_src(src),
	m_dst(dst),
	m_proto(proto),
	m_prio(prio_lo),
	m_size(size > 8 ? 8 : size)
{
	for (uint32_t i = 0; i < 8; i++)
		m_data[i] = 0;

	for (uint32_t i = 0; i < m_size; i++)
		m_data[i] = data[i];
}

frame::frame(const uint16_t src, const uint16_t dst, 
	const uint8_t proto, const vector<uint8_t> &data) :
	m_src(src),
	m_dst(dst),
	m_proto(proto),
	m_prio(prio_lo),
	m_size(0)
{
	for (uint32_t i = 0; i < 8; i++)
		m_data[i] = 0;

	for (uint32_t i = 0; i < data.size(); i++)
		m_data[i] = data[i];

	m_size = data.size();
}

frame::frame(const frame &f) :
	m_src(f.m_src),
	m_dst(f.m_dst),
	m_proto(f.m_proto),
	m_prio(f.m_prio),
	m_size(f.m_size)
{
	for (uint32_t i = 0; i < m_size; i++)
		m_data[i] = f.m_data[i];
}

frame &frame::operator = (const frame &f)
{
	m_src = f.m_src;
	m_dst = f.m_dst;
	m_proto = f.m_proto;
	m_prio = f.m_prio;
	m_size = f.m_size;
	for (uint32_t i = 0; i < m_size; i++)
		m_data[i] = f.m_data[i];
	return *this;
}

frame::~frame() throw ()
{
}

uint8_t frame::data(const int i) const
{
	return m_data[(uint32_t(i)) & 0x07];
}

void frame::set_data(const int i, const uint8_t data)
{
	m_data[(uint32_t(i)) & 0x07] = data;
	m_size = i + 1;
}

const string frame::data_as_string() const
{
	string s;
	for (size_t i = 0; i < m_size; i++)
		s += m_data[i];

	return s;
}

void frame::write_to(int fd, const in_addr_t hcand_ip) const
{
	struct sockaddr_in sin;
	int nwritten;

	uint32_t extid = (m_src & 0x3ff) |
		((m_dst & 0x3ff) << 10) |
		((m_proto & 0x07) << 20) | 
		(m_prio << 24);

	CANFrame cf;
	cf.id = extid;
	cf.size = m_size;
	for (uint32_t i = 0; i < m_size; i++)
		cf.data[i] = m_data[i];

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = hcand_ip;
	sin.sin_port = htons(3600);

	// Hoeflich sein und andere Busteilnehmer auch zum Bus lassen
	// Ausnahme: wenn extid == 0, dann ist dieses Frame fuer den hcand
	// als Keepalive bestimmt
	if (frame::polite_mode && extid != 0)
		usleep(polite_wait_time);

#ifndef __WIN32__
	
	nwritten = sendto(fd, &cf, sizeof(CANFrame), 0, 
			(struct sockaddr *) &sin, sizeof(sin));

#else
	nwritten = sendto(fd, (const char*)&cf, sizeof(CANFrame), 0, 
			(struct sockaddr *) &sin, sizeof(sin));

#endif

	if (nwritten != sizeof(CANFrame))
		throw io_error("not enough bytes written to fd: " + 
				string(strerror(errno)));

}

frame frame::read_from(int fd)
{
	CANFrame cf;

#ifdef __WIN32__
#ifndef MSG_WAITALL
#define MSG_WAITALL 0x100
#endif
	SOCKADDR_IN remoteAddr;
	int remoteLen = sizeof(SOCKADDR_IN);
	long nread = recvfrom(fd, (char*) &cf, sizeof(CANFrame), 0,
			(struct sockaddr *)&remoteAddr, &remoteLen);
	if (nread == SOCKET_ERROR)
	{
		cerr << "fd = " << fd << ", recv error, code = " << WSAGetLastError() << endl;
		throw io_error("not enough bytes read from fd to get a hcan frame");
	}
#else
	int nread = recv(fd, &cf, sizeof(CANFrame), MSG_WAITALL);
	if (nread != sizeof(CANFrame))
		throw io_error("not enough bytes read from fd to get a hcan frame");
#endif

	return frame ( (cf.id & 0x3ff), ((cf.id >> 10) & 0x3ff), 
			((cf.id >> 20) & 0x7), cf.data, cf.size);
}

void frame::set_prio(uint8_t prio)
{
	m_prio = prio;
}

const frame_message_description_t &find_frame_message_description (
		                uint8_t proto, uint8_t service, uint8_t command)
{
	for (vector<frame_message_description_t>::const_iterator i =
			frame_message_description.begin();
			i != frame_message_description.end(); i++)
	{
		if ((i->proto == proto) &&
				(i->service == service) &&
				(i->command == command))
			return *i;
	}
	throw traceable_error("find_frame_message_description: frame not found");
}


void frame::print(bool numeric, bool color, const std::string &prefix) const
{
	cout << prefix;
	cout.flush();

	if (numeric)
	{
		// numeric mode; just print the raw frame:

		cout << setbase(10) << " ";
		cout << setw(4) << setfill('0') << src() << " -> ";
		cout << setw(4) << setfill('0') << dst() << " :" <<
			setw(1) << (uint16_t)proto() << "  ["; 

		cout << setbase(16) << setfill('0') << setw(2);
		for (uint8_t i = 0; i < size(); i++)
			cout << " 0x" << setbase(16) << setfill('0') 
				<< setw(2) << (uint16_t)data(i);
		cout << " ]" << endl;
		cout << setbase(10);

	}
	else
	{
		cout << setbase(10) << " ";
		cout << setw(4) << setfill('0') << src() << " -> ";
		cout << setw(4) << setfill('0') << dst();

		if (color)
			cout << COLOR_BLUE;

		switch (proto())
		{
			case 1 : cout << " :SFP "; break;
			case 2 : cout << " :DAAP"; break;
			case 3 : cout << " :SYSLOG"; break;
			default : cout << uint16_t(proto()); break;
		}

		try
		{
			const frame_message_description_t &mdesc =
				find_frame_message_description(proto(), data(0), 
						data(1));

			cout << mdesc.service_name << " " << mdesc.command_name << " ";

			int param_index = 2;
			for (vector<string>::const_iterator i =
					mdesc.param_names.begin(); i != mdesc.param_names.end();
					i++)
			{
				if (color)
					cout << COLOR_NORMAL << COLOR_GREEN;

				cout << *i << ":";

				if (color)
					cout << COLOR_NORMAL;

				cout << (uint16_t)data(param_index++) << " ";
			}

			if (mdesc.details != "")
			{
				if (color)
					cout << COLOR_NORMAL << endl;

				cout << "      Details: " << mdesc.details;
			}
		}
		catch (const traceable_error &e)
		{
			if (color) 
				cout << COLOR_NORMAL;
			cout << " [";
			cout << setbase(16) << setfill('0') << setw(2);
			for (uint8_t i = 0; i < size(); i++)
				cout << " 0x" << setbase(16) << setfill('0')
					<< setw(2) << (uint16_t)data(i);
			cout << " ]";
			cout << setbase(10);
		}
		if (color)
			cout << COLOR_NORMAL;
		cout << endl;
		cout.flush();
	}
}

