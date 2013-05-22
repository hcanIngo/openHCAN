#include <userpanel_driver.h>
#include <assert.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <inttypes.h>

#include <eds_desc.h>
#include <eds_connection.h>
#include <eds_commands.h>

using namespace hcan;
using namespace std;

userpanel_error::userpanel_error(const string &s) :
	traceable_error(s)
{
}

userpanel_driver::userpanel_driver (board_connection &bcon, uint16_t page_size) :
	atmega32_board_driver(bcon, page_size),
	m_econ(bcon)
{
}

userpanel_driver::~userpanel_driver()
{
}

bool userpanel_driver::show_help ()
{
	atmega32_board_driver::show_help();
	hcan::eds_show_help();
	return true;
}

bool userpanel_driver::exec_command (context &c, const string &command)
{
	if (atmega32_board_driver::exec_command(c, command))
		return true;

	if (hcan::eds_exec_cmd(m_bcon, m_econ, c, command))
		return true;

	istringstream sin(command);

	string s;
	sin >> s;

	return false;
}

