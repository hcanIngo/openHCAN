#include <usv_driver.h>
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

usv_error::usv_error(const string &s) :
	traceable_error(s)
{
}

usv_driver::usv_driver (board_connection &bcon) :
	atmega32_board_driver(bcon)
{
}

usv_driver::~usv_driver()
{
}

bool usv_driver::show_help ()
{
	atmega32_board_driver::show_help();

	cout << 
		"	show usvstate\n" <<
		"	reset-usvstats\n" <<
		endl;

	return true;
}

void usv_driver::print_U_stats(uint16_t n, const string &name)
{
	uint8_t Ux_lo,Ux_hi;
	uint16_t Ux;
	float U;

	m_tcon.send_VOLTAGE_REQUEST(m_bcon.src(), m_bcon.dst(),n);
	m_tcon.recv_VOLTAGE_REPLAY(m_bcon.dst(), m_bcon.src(), 
			&Ux_hi, &Ux_lo);
	Ux = Ux_lo | (Ux_hi << 8);
	U  = 30.0 / 1024.0 * Ux;

	uint8_t dummy, min_hi, min_lo, max_hi, max_lo;
	m_tcon.send_VOLTAGE_STATS_REQUEST(m_bcon.src(), m_bcon.dst(),n);
	m_tcon.recv_VOLTAGE_STATS_REPLAY(m_bcon.dst(), m_bcon.src(), 
			&dummy, &min_hi, &min_lo, &max_hi, &max_lo);
	float Umin = 30.0 / 1024.0 * (min_lo | (min_hi << 8));
	float Umax = 30.0 / 1024.0 * (max_lo | (max_hi << 8));

	cout << "U(" << name << "): \t";
	cout << setiosflags(ios::fixed) << setprecision(1) << setw(4);
	cout << U << " V" << endl;
	cout << " min :      \t" << Umin << " V" << endl;
	cout << " max :      \t" << Umax << " V" << endl;

	cout << endl;
}

bool usv_driver::exec_command (context &c, const string &command)
{
	if (atmega32_board_driver::exec_command(c, command))
		return true;

	istringstream sin(command);

	string s;
	sin >> s;

	if (s == "show")
	{
		sin >> s;

		if (s == "usvstate")
		{
			uint8_t state;
			m_tcon.send_STATE_REQUEST(m_bcon.src(), m_bcon.dst());
			m_tcon.recv_STATE_INFO(m_bcon.dst(), m_bcon.src(), &state);

			if (state)
				cout << "USV Status: Batterie-Betrieb\n";
			else
				cout << "USV Status: Netz-Betrieb\n";
			
			cout << endl;

			print_U_stats(1,"Netzteil");
			print_U_stats(2,"Bat 1");
			print_U_stats(3,"Bat 1+2");
			print_U_stats(4,"Puffer");

			return true;
		}
	}

	if (s == "reset-usvstats")
	{
		m_tcon.send_VOLTAGE_STATS_RESET(m_bcon.src(), m_bcon.dst());
		return true;
	}

	return false;
}


