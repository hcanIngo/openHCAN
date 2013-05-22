#include <atmega32_board_driver.h>
#include <board_driver.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <assert.h>

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace hcan;
using namespace std;


atmega32_board_driver::atmega32_board_driver (board_connection &bcon, uint16_t page_size) :
	board_driver(bcon)
{
	m_page_size = page_size;
}

atmega32_board_driver::~atmega32_board_driver()
{
}

bool atmega32_board_driver::show_help ()
{
	board_driver::show_help();

	cout << 
		"	flash <filename>\n" << endl;

	return true;
}

bool atmega32_board_driver::exec_command (context &c, const string &command)
{
	if (board_driver::exec_command(c,command))
		return true;

	istringstream sin(command);

	string s;
	sin >> s;

	if (s == "flash")
	{
		string filename;
		sin >> filename;

		cmd_flash(filename);

		return true;
	}

	return false;
}

