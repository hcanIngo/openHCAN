#include <transport_connection.h>
#include <frame.h>
#include <traceable_error.h>

#include <frame_message_description.h>
#include <../telican/installation_data.h>

#include <hcan_multicast.h>
#include <hcan.h>
#include <colors.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#ifndef __WIN32__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#else

#include <winsock2.h>
#include <io.h>

#endif

#include <cstdio> //fuer g++-4.4: perror
#include <errno.h>
#include <sys/time.h>
#include <signal.h>

using namespace hcan;
using namespace std;



transport_error::transport_error (const string &s) :
	traceable_error(s)
{
}


transport_connection::transport_connection (const in_addr_t hcand_ip) :
	hcan_transport(hcand_ip)
{
	if (m_hcand_ip == 0)
	{
		// Caller wuenscht den Defaultwert; also 127.0.0.1
		m_hcand_ip = inet_addr("127.0.0.1");
	}

	m_socket = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == -1)
		throw transport_error ("failed to open socket: " + 
				string(strerror(errno)));
	keep_connection_alive();
}

void transport_connection::keep_connection_alive()
{
	frame f(0,0,0, vector<uint8_t>());
	f.set_prio(frame::prio_hi);
	f.write_to(m_socket,m_hcand_ip);
}

transport_connection::~transport_connection() throw ()
{
	close (m_socket);
#ifdef __WIN32__
	WSACleanup();
#endif
}

frame transport_connection::recv_frame (const uint16_t dst)
{
	/*
	 * On Linux, the function select() modifies timeout to reflect the amount
	 * of  time not slept; most other implementations do not do this.  This
	 * causes problems both when Linux code which reads timeout is ported to
	 * other  operating  systems,  and when code is ported to Linux that reuses
	 * a struct timeval for multiple select()s in a loop without reinitializing
	 * it.  Consider timeout  to  be  undefined  after select() returns.
	 *
	 * -> fix this, when we port it to other platforums 
	 *
	 */

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 800000; // bis 2016-12-04:  500000;
	/* IL: vergroessert, da grosse Firmware
	(padded size 25856 byte) zum Timeout fuehrte; ex: 300000
	"warning: no firmware crc16 check support (update bootloader!)" */

	// Normally, the counter does not reach 0, but if something bad happens
	// this is our safety to not get stuck...
	int counter = 100000; 

	while (counter--)
	{
		fd_set rfds;
		int retval;

		FD_ZERO(&rfds);
		FD_SET((uint32_t)m_socket, &rfds); //neuen Socket (file descriptor) setzen

		retval = select(m_socket + 1, &rfds, NULL, NULL, &tv);

		if (retval == -1)
		{
			perror ("select()");
		}

		if (retval > 0)
		{
			frame f = frame::read_from(m_socket);
			if (f.dst() != dst)
				continue;

			return f;
		}
		else
		{
			throw transport_error ("timeout while waiting for packet");
	}
	}

	throw transport_error ("timeout while waiting for packet");
}


void transport_connection::ping(uint16_t src, uint16_t dst)
{
	cout << "sending ping packets from " << src << " to " 
		<< dst << "..." << endl;

	for (int i = 0; i < 5; i++)
	{
		try
		{
#ifndef __WIN32__
			struct timeval starttv,endtv;
			gettimeofday(&starttv,0);
#endif

			send_PING_REQUEST(src,dst);
			recv_PING_REPLAY(dst,src);

#ifndef __WIN32__
			gettimeofday(&endtv,0);

			uint64_t diff = (endtv.tv_sec * 1000000 + endtv.tv_usec) -
				(starttv.tv_sec * 1000000 + starttv.tv_usec);

			cout << " [" << (i + 1) << "] " << (diff / 1000) << " msec";
#else
			cout << " [" << (i + 1) << "] ";
#endif

		}
		catch (const transport_error &e)
		{
			// we are pinging, so ignore possible timeouts
			keep_connection_alive();
		}
#ifndef __WIN32__
		usleep (200000);
#else
		_sleep(200);
#endif
		cout << endl;
	}
}

void transport_connection::flood_ping(uint16_t src, uint16_t dst, 
		const int load)
{
	cout << "sending flood ping packets from " << src << " to " 
		<< dst << "..." << endl;

	for (int i = 0; i < 20; i++)
	{
		try
		{
			struct timeval starttv,endtv;
			gettimeofday(&starttv,0);

			for (int j  = 0; j < load; j++)
				send_PING_REQUEST(src,dst);
			for (int j  = 0; j < load; j++)
				recv_PING_REPLAY(dst,src);

			gettimeofday(&endtv,0);

			uint64_t diff = (endtv.tv_sec * 1000000 + endtv.tv_usec) -
				(starttv.tv_sec * 1000000 + starttv.tv_usec);

			cout << " [" << (i + 1) << "] " << (diff / 1000) << " msec";

		}
		catch (const transport_error &e)
		{
			// we are pinging, so ignore possible timeouts
			keep_connection_alive();
		}
#ifndef __WIN32__
		usleep (200000);
#else
		_sleep(200);
#endif
		cout << endl;
	}
}

void transport_connection::ping_once(uint16_t src, uint16_t dst)
{
	cout << "sending one ping packet from " << src << " to " 
		<< dst << "..." << endl;

	try
	{
#ifndef __WIN32__
		struct timeval starttv,endtv;
		gettimeofday(&starttv,0);
#endif

		send_PING_REQUEST(src,dst);
		recv_PING_REPLAY(dst,src);

#ifndef __WIN32__
		gettimeofday(&endtv,0);

		uint64_t diff = (endtv.tv_sec * 1000000 + endtv.tv_usec) -
			(starttv.tv_sec * 1000000 + starttv.tv_usec);

		cout << " [" << (1) << "] " << (diff / 1000) << " msec";
#else
		cout << " [" << (1) << "] ";
#endif

	}
	catch (const transport_error &e)
	{
		// we are pinging, so ignore possible timeouts
		keep_connection_alive();
	}
#ifndef __WIN32__
	usleep (200000);
#else
	_sleep(200);
#endif
	cout << endl;
}


volatile bool done = false;

void sig_handler(int)
{
	done = true;
}

frame transport_connection::recv_frame_with_timeout(int seconds)
{
	struct timeval tv;
	tv.tv_sec = seconds; // wait for at most 1 second
	tv.tv_usec = 0;

	fd_set rfds;
	int retval;

	FD_ZERO(&rfds);
	FD_SET((uint32_t)m_socket, &rfds);

	retval = select(m_socket + 1, &rfds, NULL, NULL, &tv);

	if (retval == -1)
		perror ("select()");

	if (! retval)
		throw transport_error ("timeout while waiting for packet");

	return frame::read_from(m_socket);
}

void transport_connection::templog(bool resolve)
{
	if (resolve)
	{
		try
		{
			global_installation_data.load(INSTALLATION_XML);
		}
		catch (std::exception &e)
		{
			cout << "cannot resolve: installation.xml not found" << endl;
			resolve = false;
		}
	}

	while (! done)
	{
		try
		{
			const frame f = recv_frame_with_timeout(1);
			keep_connection_alive();

			if ((f.proto() == HCAN_PROTO_SFP) && 
					(f.dst() == HCAN_MULTICAST_INFO) &&
					(f.data(0) == HCAN_SRV_HES) &&
					(f.data(1) == HCAN_HES_1WIRE_TEMPERATURE ))
			{
				string name = "";

				const uint8_t hi = f.data(3);
				const uint8_t lo = f.data(4);

				int16_t temp = (hi << 8)| lo;
				float ftemp = temp;
				ftemp /= 16;

				if (resolve)
				{
					try
					{
						name = " name: \"" + global_installation_data.tempsensor_name_by_gruppe(f.data(2)) + "\"";
					}
					catch (const traceable_error &e)
					{
						name = "";
					}
				}

					cout << "group: " << (uint16_t)f.data(2) << name << " temp: " << ftemp << endl;
			}
		}
		catch (const transport_error &e)
		{
			keep_connection_alive();
		}
	}
}

typedef struct 
{ 
	uint8_t prio; 
	string msg; 
} message_entry;

void transport_connection::syslog(bool resolve)
{
	if (resolve)
	{
		try
		{
			global_installation_data.load(INSTALLATION_XML);
		}
		catch (std::exception &e)
		{
			cout << "cannot resolve: installation.xml not found" << endl;
			resolve = false;
		}
	}

	map<uint16_t, message_entry> messages;

	while (! done)
	{
		try
		{
			const frame f = recv_frame_with_timeout(1);
			keep_connection_alive();

			if ((f.proto() == 3) && (f.dst() == HCAN_MULTICAST_SYSLOG))
			{
				// first, look if we already have an open string stored
				// from this peer:

				map<uint16_t, message_entry>::iterator it = messages.find(f.src());
				if (it !=messages.end())
				{
					// yes, there is already something; now add the
					// string received

					if(f.data_as_string()[0] == it->second.prio)
						it->second.msg += f.data_as_string().substr(1);
					else
						it->second.msg += f.data_as_string();

					// now check it the string we have built is complete:
					const string s = it->second.msg;

					if (s.size() && (s[s.size()-1] == '\n'))
					{
						// yes, we found the RETURN symbol at the end
						// now, print the message:
						//

						string prio;
						switch (it->second.prio)
						{
							case 1 : prio = "CRITICAL"; break;
							case 2 : prio = "ERROR"; break;
							case 3 : prio = "Warning"; break;
							case 4 : prio = "debug"; break;
						}
						const time_t now = time(0);
						const struct tm *tm = localtime(&now);

						char timestamp[32];
						strftime(timestamp,sizeof(timestamp), "%b %d %H:%M:%S",
								tm);

						string name = "";
						if (resolve)
						{
							try
							{
								name = " (\"" + global_installation_data.board_name_by_addr(f.src()) + "\")";
							}
							catch (const traceable_error &e)
							{
								name = "";
							}
						}

						cout << timestamp << " " << f.src() << name << "  " << prio << "  " << s;
						cout.flush();

						// delete it from out messages hash
						messages.erase(it);
					}
				}
				else
				{
					// no, it is the first packet
					messages[f.src()].msg = f.data_as_string().substr(1);
					messages[f.src()].prio = f.data_as_string()[0];
				}
			}
		}
		catch (const transport_error &e)
		{
			keep_connection_alive();
		}
	}
}

void transport_connection::dump(bool numeric, bool color, bool syslog, bool resolve)
{
	// this fills the frame_message_description vector
	// (see mk_frame_message_description_h.xsl for details)
	init_frame_message_description();

	if (resolve)
	{
		try
		{
			global_installation_data.load(INSTALLATION_XML);
		}
		catch (std::exception &e)
		{
			cout << "cannot resolve: installation.xml not found" << endl;
			resolve = false;
		}
	}

	int count = 0;
	while (! done)
	{
		try
		{
			const frame f = frame::read_from(m_socket);
			keep_connection_alive();
			if ((!syslog) && (f.proto() == 3) && (f.dst() == HCAN_MULTICAST_SYSLOG))
			{
				continue;
			}

			f.print(numeric,color,resolve,"");

			count++;
		}
		catch (const transport_error &e)
		{
			keep_connection_alive();
		}
	}
	cerr << count << " frames sniffed." << endl;
}

void transport_connection::send(const frame &f)
{
    f.write_to(m_socket, m_hcand_ip);
}

