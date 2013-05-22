#ifndef OAP_DRIVER_H
#define OAP_DRIVER_H

#include <atmega32_board_driver.h>
#include <libhcan++/traceable_error.h>
#include <libhcan++/eds_connection.h>
#include <libhcan++/oap_access.h>

namespace hcan
{
	class oap_error : public traceable_error
	{
		public:
			oap_error(const string &);
	};

	class oap_driver
	{
		protected:
			//eds_connection m_econ;
			oap_access m_oap;
		protected:
			void cmd_walk();
		public:
			oap_driver (board_connection &bcon);
			virtual ~oap_driver();
			virtual bool show_help ();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
