#ifndef USERPANEL_DRIVER_H
#define USERPANEL_DRIVER_H

#include <atmega32_board_driver.h>
#include <libhcan++/traceable_error.h>
#include <libhcan++/eds_connection.h>

namespace hcan
{
	class userpanel_error : public traceable_error
	{
		public:
			userpanel_error(const string &);
	};

	class userpanel_driver : public atmega32_board_driver 
	{
		private:
			eds_connection m_econ;
		protected:
			virtual bool show_help ();
		public:
			userpanel_driver (board_connection &bcon, uint16_t page_size=128);
			virtual ~userpanel_driver();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
