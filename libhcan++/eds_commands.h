#ifndef EDS_COMMANDS_H
#define EDS_COMMANDS_H

#include <libhcan++/board_driver.h>
#include <libhcan++/eds_connection.h>

namespace hcan
{
	bool eds_exec_cmd(board_connection &bcon, eds_connection &econ, 
			context &c, const string &cmd);
	bool eds_show_help ();
};

#endif
