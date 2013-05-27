#include "rolladen_command.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <hcan_multicast.h>

rolladen_command::rolladen_command(hcan::transport_connection &tcon, uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

rolladen_command::~rolladen_command()
{
}

bool rolladen_command::exec(const string &line)
{
	boost::cmatch matches;

	// Erste Befehlsform:
	boost::regex re1("^rolladen *([[:word:]-]+) *([0-9]+) *$");
	if (boost::regex_match(line.c_str(), matches, re1))
	{
		// Lampen-Gruppe besorgen:
		uint8_t gruppe = global_installation_data.
			rolladen_gruppe_by_name(matches[1]);

		// Lampe ein- oder ausschalten:
		int pos = boost::lexical_cast<int>(matches[2]);
		m_tcon.send_ROLLADEN_POSITION_SET(m_src,
					HCAN_MULTICAST_CONTROL,gruppe,pos);
		return true;
	}

	// Zweite Befehlsform:
	boost::regex re2("^rolladen *$");
	if (boost::regex_match(line.c_str(), matches, re2))
	{
		cout << "Gruppe:  Status:   Name:" << endl;
		vector<string> rolladen = global_installation_data.rollaeden_names();
		for (vector<string>::iterator i = rolladen.begin();
				i != rolladen.end(); i++)
		{
			// Lampen-Gruppe besorgen:
			uint8_t gruppe = global_installation_data.
				rolladen_gruppe_by_name(*i);

			uint8_t pos;
			m_tcon.send_ROLLADEN_POSITION_REQUEST(m_src,
					HCAN_MULTICAST_CONTROL,gruppe);
			m_tcon.recv_ROLLADEN_POSITION_REPLAY(0, m_src, &gruppe, &pos);


			cout << setw(4) << setfill(' ') << (int)gruppe
				<< "     " << setw(4) << setfill(' ') << (int)pos
				<< "      " << *i << endl;
		}

		return true;
	}

	return false;
}

