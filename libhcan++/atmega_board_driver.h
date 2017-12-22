#ifndef atmega_board_driver_H
#define atmega_board_driver_H

#include <memory>
#include <board_driver.h>

namespace hcan
{
	class atmega_board_driver : public board_driver
	{
		protected:
			virtual bool show_help ();
		public:
			atmega_board_driver (board_connection &bcon, uint16_t page_size=128);
			virtual ~atmega_board_driver ();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
