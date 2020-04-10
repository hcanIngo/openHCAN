#include "lampe_command.h"
#include <boost/regex.hpp>
#include <iostream>
#include <hcan_multicast.h>

lampe_command::lampe_command(hcan::transport_connection &tcon, uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

lampe_command::~lampe_command()
{
}

bool lampe_command::exec(const string &line)
{
	try
	{
		boost::cmatch matches;

		// Erste Befehlsform:
		boost::regex re1("^lampe *([[:word:]-]+) *(0|1|ein|an|aus|on|off) *$");
		if (boost::regex_match(line.c_str(), matches, re1))
		{
			// Lampen-Gruppe besorgen:
			uint8_t gruppe = global_installation_data.lampe_gruppe_by_name(matches[1]);

			// Lampe ein- oder ausschalten:
			if ((matches[2] == "1") ||
					(matches[2] == "ein") ||
					(matches[2] == "an") ||
					(matches[2] == "on"))
			{
				m_tcon.send_POWER_GROUP_ON(m_src,HCAN_MULTICAST_CONTROL,gruppe);
			}
			else
			{
				m_tcon.send_POWER_GROUP_OFF(m_src,HCAN_MULTICAST_CONTROL,gruppe);
			}
			return true;
		}

		// Zweite Befehlsform:
		boost::regex re2("^lampe *$");
		if (boost::regex_match(line.c_str(), matches, re2))
		{
			cout << "Gruppe:  Status:   Name:" << endl;
			vector<string> lampen = global_installation_data.lampen_names();
			for (vector<string>::iterator i = lampen.begin();
					i != lampen.end(); i++)
			{
				// Lampen-Gruppe besorgen:
				uint8_t gruppe_query = global_installation_data.lampe_gruppe_by_name(*i);

				uint8_t status;
				uint8_t timer;
				m_tcon.send_POWER_GROUP_STATE_QUERY(m_src,
						HCAN_MULTICAST_CONTROL,gruppe_query);

				uint8_t gruppe_replay;
				// Da hier theoretisch auch das Replay des vorherigen Querys kommen kann,
				// wird der Recive Vorgang solange ausgefuehrt bis die richtige Gruppe kommt.
				do {
					m_tcon.recv_POWER_GROUP_STATE_REPLAY(0, m_src,
							&gruppe_replay, &status, &timer);
				} while (gruppe_query != gruppe_replay);


				cout << setw(4) << setfill(' ') << (int)gruppe_replay
					<< "     " << (status != 0 ? "ein" : " - ")
					<< "      " << *i << endl;
			}

			return true;
		}
	}
	catch (traceable_error &e)
	{
		cerr << e.what() << endl;
	}

	return false;
}

