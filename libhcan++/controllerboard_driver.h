#ifndef controllerboard_DRIVER_H
#define controllerboard_DRIVER_H

#include <atmega_board_driver.h>
#include <libhcan++/traceable_error.h>
#include <libhcan++/eds_connection.h>
#include <libhcan++/oap_driver.h>

namespace hcan
{
	class controllerboard_error : public traceable_error
	{
		public:
			controllerboard_error(const string &);
	};

	class controllerboard_driver : public atmega_board_driver 
	{
		protected:
			eds_connection m_econ;
			oap_driver m_oap_driver;
		protected:
			virtual bool show_help ();
			virtual void cmd_show_ram_usage();
			virtual void cmd_discover_1wire(uint8_t pin);
			virtual void cmd_config_reload();
		public:
			controllerboard_driver (board_connection &bcon, uint16_t page_size=128);
			virtual ~controllerboard_driver();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
