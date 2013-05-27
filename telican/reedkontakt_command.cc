#include "reedkontakt_command.h"
#include <boost/regex.hpp>
#include <iostream>
#include <hcan_multicast.h>

reedkontakt_command::reedkontakt_command(hcan::transport_connection &tcon, 
		uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

reedkontakt_command::~reedkontakt_command()
{
}

bool reedkontakt_command::exec(const string &line)
{
	boost::cmatch matches;

	boost::regex re1("^reedkontakt *$");
	if (boost::regex_match(line.c_str(), matches, re1))
	{
		cout << "Gruppe:  Status:   Name:" << endl;
		vector<string> reedkontakte = global_installation_data.
			reedkontakte_names();
		for (vector<string>::iterator i = reedkontakte.begin();
				i != reedkontakte.end(); i++)
		{
			// Lampen-Gruppe besorgen:
			uint8_t gruppe = global_installation_data.
				reedkontakt_gruppe_by_name(*i);

			uint8_t status;
			m_tcon.send_REEDKONTAKT_STATE_QUERY(m_src,
					HCAN_MULTICAST_CONTROL,gruppe);
			m_tcon.recv_REEDKONTAKT_STATE_REPLAY(0, m_src, &gruppe, &status);


			cout << setw(4) << setfill(' ') << (int)gruppe
				<< "     " << (status != 0 ? "offen" : " - ")  
				<< "      " << *i << endl;
		}

		return true;
	}

	return false;
}

