#ifndef ATMEGA8_BOARD_DRIVER_H
#define ATMEGA8_BOARD_DRIVER_H

#include <memory>
#include <board_driver.h>

namespace hcan
{
	class atmega8_board_driver : public board_driver
	{
		protected:
			virtual bool show_help ();
		public:
			atmega8_board_driver (board_connection &bcon, uint16_t page_size);
			virtual ~atmega8_board_driver ();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
