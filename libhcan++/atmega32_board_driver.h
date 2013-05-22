#ifndef ATMEGA32_BOARD_DRIVER_H
#define ATMEGA32_BOARD_DRIVER_H

#include <memory>
#include <board_driver.h>

namespace hcan
{
	class atmega32_board_driver : public board_driver
	{
		protected:
			virtual bool show_help ();
		public:
			atmega32_board_driver (board_connection &bcon, uint16_t page_size=128);
			virtual ~atmega32_board_driver ();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
