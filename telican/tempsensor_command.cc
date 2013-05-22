#include "tempsensor_command.h"
#include <boost/regex.hpp>
#include <iostream>
#include <hcan_multicast.h>

tempsensor_command::tempsensor_command(hcan::transport_connection &tcon, 
		uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

tempsensor_command::~tempsensor_command()
{
}

bool tempsensor_command::exec(const string &line)
{
	boost::cmatch matches;

	boost::regex re1("^tempsensor *$");
	if (boost::regex_match(line.c_str(), matches, re1))
	{
		cout << "Gruppe: Temperatur:   Name:" << endl;
		vector<string> tempsensors = global_installation_data.
			tempsensor_names();
		for (vector<string>::iterator i = tempsensors.begin();
				i != tempsensors.end(); i++)
		{
			// Lampen-Gruppe besorgen:
			uint8_t gruppe = global_installation_data.
				tempsensor_gruppe_by_name(*i);

			uint8_t temp_hi;
			uint8_t temp_lo;
			m_tcon.send_1WIRE_TEMPERATURE_QUERY(m_src,
					HCAN_MULTICAST_CONTROL,gruppe);
			m_tcon.recv_1WIRE_TEMPERATURE_REPLAY(0, m_src, &gruppe, &temp_hi, &temp_lo);

      int16_t temp = (temp_hi << 8)| temp_lo;
      float ftemp = temp;
      ftemp /= 16;

      cout.setf(ios_base::right,ios_base::adjustfield);
			cout << setw(4) << setfill(' ') << (int)gruppe << "     ";
			cout.setf(ios_base::left,ios_base::adjustfield);
			cout << setw(9) << setfill(' ') << ftemp << "     ";
			cout << *i << endl;
		}

		return true;
	}

	return false;
}

