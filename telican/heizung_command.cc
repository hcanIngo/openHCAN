#include "heizung_command.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <hcan_multicast.h>
#include <hcan-def.h>

heizung_command::heizung_command(hcan::transport_connection &tcon, uint16_t src) :
	m_tcon(tcon),
	m_src(src)
{
}

heizung_command::~heizung_command()
{
}

float heizung_command::get_ist_temp(uint8_t id)
{
	uint8_t dummy,hi,lo;
	m_tcon.send_HEIZUNG_TIST_REQUEST(m_src, HCAN_MULTICAST_CONTROL,id);
	m_tcon.recv_HEIZUNG_TIST_REPLAY(0,m_src,&dummy,&hi,&lo);
	return ((hi << 8) | lo) / 16.0;
}

heizung_info_t heizung_command::get_details(uint8_t id)
{
	heizung_info_t info;
	m_tcon.send_HEIZUNG_DETAILS_REQUEST(m_src, HCAN_MULTICAST_CONTROL,id);

	hcan::frame f = m_tcon.recv_frame(m_src);

	switch (f.data(1))
	{
		case HCAN_HES_HEIZUNG_MODE_OFF_DETAILS :
			info.mode = 0;
			break;
		case HCAN_HES_HEIZUNG_MODE_MANUAL_DETAILS :
			info.mode = 1;
			info.stellgrad = f.data(3);
			info.dauer = (f.data(4)<<8 | f.data(5));
			info.dauer_min = info.dauer / 60;
			info.dauer_sec = info.dauer % 60;
			break;
		case HCAN_HES_HEIZUNG_MODE_THERMOSTAT_DETAILS :
			info.mode = 2;
			info.soll_temp = ((f.data(3)<<8)|f.data(4))/16.0;
			info.ist_temp = get_ist_temp(id);
			info.dauer = (f.data(5)<<8) | f.data(6);
			info.dauer_min = info.dauer / 60;
			info.dauer_sec = info.dauer % 60;
			break;
		case HCAN_HES_HEIZUNG_MODE_AUTOMATIK_DETAILS :
			info.mode = 3;
			info.soll_temp = (f.data(3)<<8|f.data(4))/16.0;
			info.zz_id = f.data(5);
			break;
		default:
			throw traceable_error("unknown heizung mode");
	}

	info.ist_temp = get_ist_temp(id);

	stringstream ss;
	ss << setw(3) << info.dauer_min << ":" << setw(2) << setfill('0') 
		<< info.dauer_sec << "min";
	info.dauer_str = ss.str();

	return info;
}

void heizung_command::print_header()
{
	cout << "|  ID | Modus   | T(ist) | T(soll) | StGrd | Dauer     | Ort " 
		<< endl;
	cout << "+-----+---------+--------+---------+-------+-----------+----------" 
		<< endl;
}

void heizung_command::print_heizung(uint8_t id)
{
	cout << "| " << setw(3) << uint16_t(id) << " | ";
	heizung_info_t info = get_details(id);

	switch (info.mode)
	{
		case HEIZUNG_MODE_OFF :
			cout << "aus     | " 
				<< setiosflags(ios::fixed) << setprecision(1) << setw(4)
				<< info.ist_temp
				<< "   |         |       |           | ";
			break;
		case HEIZUNG_MODE_MANUELL :
			cout << "manuell | "
				<< setiosflags(ios::fixed) << setprecision(1) << setw(4)
				<< info.ist_temp
		 		<< "   |         | " << setw(3) 
				<< info.stellgrad << "   | "
				<< info.dauer_str << " | ";
			break;
		case HEIZUNG_MODE_THERMOSTAT :
			cout << "therm.  | " 
				<< setiosflags(ios::fixed) << setprecision(1) 
				<< setfill('0') << setw(4)
				<< info.ist_temp << "   | "
				<< info.soll_temp <<  "    |       | "
			   	<< info.dauer_str << " | ";
			break;
		case HEIZUNG_MODE_AUTO :
			cout << "auto    | " 
				<< setiosflags(ios::fixed) << setprecision(1) << setw(4)
				<< setfill('0')
				<< info.ist_temp << "   | "
				<< setiosflags(ios::fixed) << setprecision(1) << setw(4)
				<< setfill('0')
				<< info.soll_temp <<  "    |       |           | ";
			break;
	}

	cout << global_installation_data.heizung_name_by_id(id) << endl;

}

void heizung_command::print_heizungen()
{
	print_header();

	vector<string> heizungen = global_installation_data.heizungen_names();
	for (vector<string>::iterator i = heizungen.begin();
			i != heizungen.end(); i++)
	{
		uint8_t id = global_installation_data.
			heizung_id_by_name(*i);

		print_heizung(id);
	}
}

bool heizung_command::exec(const string &line)
{
	boost::cmatch matches;

	boost::regex re("^heizung *$");
	if (boost::regex_match(line.c_str(), matches, re))
	{
		print_heizungen();
		return true;
	}

	boost::regex re2("^heizung *([[:word:]-]+) *$");
	if (boost::regex_match(line.c_str(), matches, re2))
	{
		print_header();
		print_heizung(global_installation_data.heizung_id_by_name(
					matches[1]));
		return true;
	}

	boost::regex re3("^heizung *([[:word:]-]+) +auto *$");
	if (boost::regex_match(line.c_str(), matches, re3))
	{
		uint8_t id = global_installation_data.heizung_id_by_name(
				matches[1]);
		m_tcon.send_HEIZUNG_SET_MODE_AUTOMATIK(m_src, 
				HCAN_MULTICAST_CONTROL,id);
		return true;
	}

	boost::regex re4("^heizung *([[:word:]-]+) +aus *$");
	if (boost::regex_match(line.c_str(), matches, re4))
	{
		uint8_t id = global_installation_data.heizung_id_by_name(
				matches[1]);
		m_tcon.send_HEIZUNG_SET_MODE_OFF(m_src, 
				HCAN_MULTICAST_CONTROL,id);
		return true;
	}

	boost::regex re5("^heizung *([[:word:]-]+) +manuell +([0-9]+) +([0-9]+) *$");
	if (boost::regex_match(line.c_str(), matches, re5))
	{
		uint8_t id = global_installation_data.heizung_id_by_name(
				matches[1]);
		uint8_t stellgrad = boost::lexical_cast<int>(matches[2]);
		uint16_t dauer = boost::lexical_cast<int>(matches[3]) * 60; // Sekunden!
		m_tcon.send_HEIZUNG_SET_MODE_MANUAL(m_src, 
				HCAN_MULTICAST_CONTROL,id,stellgrad,dauer>>8,dauer);
		return true;
	}

	boost::regex re6("^heizung *([[:word:]-]+) +therm +([0-9\\.]+) +([0-9]+) *$");
	if (boost::regex_match(line.c_str(), matches, re6))
	{
		uint8_t id = global_installation_data.heizung_id_by_name(
				matches[1]);
		float temp = boost::lexical_cast<float>(matches[2]);
		uint8_t hi = (uint16_t)(temp * 16) >> 8;
		uint8_t lo = (uint16_t)(temp * 16);
		uint16_t dauer = boost::lexical_cast<int>(matches[3]) * 60; // Sekunden!
		m_tcon.send_HEIZUNG_SET_MODE_THERMOSTAT_DETAILS(m_src, 
				HCAN_MULTICAST_CONTROL,id,hi,lo,dauer>>8,dauer);
		return true;
	}

	return false;
}

