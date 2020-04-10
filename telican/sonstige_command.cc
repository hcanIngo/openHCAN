#include "sonstige_command.h"
#include <boost/regex.hpp>
#include <iostream>
#include <hcan_multicast.h>

sonstige_command::sonstige_command(hcan::transport_connection &tcon, uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

sonstige_command::~sonstige_command()
{
}

bool sonstige_command::exec(const string &line)
{
	try
	{
		boost::cmatch matches;

		// Erste Befehlsform:
		boost::regex re1("^sonstige *([[:word:]-]+) *(0|1|ein|an|aus|on|off) *$");
		if (boost::regex_match(line.c_str(), matches, re1))
		{
			// "Powerport"-Gruppe besorgen:
			uint8_t gruppe = global_installation_data.sonstige_gruppe_by_name(matches[1]);

			// Sonstige ein- oder ausschalten:
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
		boost::regex re2("^sonstige *$");
		if (boost::regex_match(line.c_str(), matches, re2))
		{
			cout << "Gruppe:  Status:   Name:" << endl;
			vector<string> sonstige = global_installation_data.sonstige_names();
			for (vector<string>::iterator i = sonstige.begin();
					i != sonstige.end(); i++)
			{
				// Sonstige-Gruppe besorgen:
				uint8_t gruppe = global_installation_data.sonstige_gruppe_by_name(*i);

				uint8_t status;
				uint8_t timer;
				m_tcon.send_POWER_GROUP_STATE_QUERY(m_src,
						HCAN_MULTICAST_CONTROL,gruppe);
				m_tcon.recv_POWER_GROUP_STATE_REPLAY(0, m_src, &gruppe, &status, &timer);


				cout << setw(4) << setfill(' ') << (int)gruppe
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

