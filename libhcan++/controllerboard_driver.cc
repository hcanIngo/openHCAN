#include <controllerboard_driver.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <eds_commands.h>
#include <assert.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <inttypes.h>

using namespace hcan;
using namespace std;

controllerboard_error::controllerboard_error(const string &s) :
	traceable_error(s)
{
}

controllerboard_driver::controllerboard_driver (board_connection &bcon, uint16_t page_size) :
	atmega_board_driver(bcon, page_size),
	m_econ(bcon),
	m_oap_driver(bcon)
{
}

controllerboard_driver::~controllerboard_driver()
{
}

bool controllerboard_driver::show_help ()
{
	atmega_board_driver::show_help();
	hcan::eds_show_help();
	m_oap_driver.show_help();

	cout << 
		"	discover1wire <pin>\n" <<
		"	show ram-usage\n" <<
		"	reload\n" << 
		
		endl;

	return true;
}


bool controllerboard_driver::exec_command (context &c, const string &command)
{
	if (atmega_board_driver::exec_command(c, command))
		return true;

	if (hcan::eds_exec_cmd(m_bcon, m_econ, c, command))
		return true;

	istringstream sin(command);

	string s;
	sin >> s;

	if (s == "show")
	{
		sin >> s;

		if (s == "ram-usage")
		{
			cmd_show_ram_usage();
			return true;
		}

	}

	if (s == "discover1wire")
	{
		uint16_t pin;
		sin >> pin;

		cmd_discover_1wire(pin);
		return true;
	}
	if (s == "reload")
	{
		cmd_config_reload();	
		return true;
	}

	if (m_oap_driver.exec_command(c, command))
		return true;

	return false;
}


void controllerboard_driver::cmd_discover_1wire(uint8_t pin)
{
	uint8_t id[8];

	m_tcon.send_1WIRE_DISCOVER(m_bcon.src(), m_bcon.dst(), pin);

	try
	{
		while (1)
		{
			m_tcon.recv_1WIRE_DISCOVERED_PART_1(
					m_bcon.dst(), m_bcon.src(), 
					&id[0], &id[1], &id[2], &id[3] );

			m_tcon.recv_1WIRE_DISCOVERED_PART_2(
					m_bcon.dst(), m_bcon.src(), 
					&id[4], &id[5], &id[6], &id[7] );

			switch (id[0])
			{
                case 0x28 : cout << "DS18B20 temperature sensor:   "; break;
                case 0x10 : cout << "DS18S20 temperature sensor:   "; break;
				default :   cout << "unknown 1wire device:        "; break;
			}
			cout <<  setbase(16) << setw(2) << setfill('0');
			for (int i = 0; i < 8; i++)
				cout << setw(2) << setfill('0') << uint16_t(id[i]);
			cout << setbase(10) << setw(1) << endl;
		}
	}
	catch (const transport_error &e)
	{
	}
}

void controllerboard_driver::cmd_show_ram_usage()
{
	uint8_t hi, lo;
	m_tcon.send_CONFIG_RAM_USAGE_REQUEST( m_bcon.src(), m_bcon.dst() );
	m_tcon.recv_CONFIG_RAM_USAGE_REPLAY( m_bcon.dst(), m_bcon.src(), &hi, &lo );

	cout << "config ram usage: " << (uint16_t)(hi << 8 | lo) 
		<< " bytes" << endl;

}

void controllerboard_driver::cmd_config_reload()
{
	m_tcon.send_CONFIG_RELOAD( m_bcon.src(), m_bcon.dst() );
}

