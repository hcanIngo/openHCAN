#ifndef USV_DRIVER_H
#define USV_DRIVER_H

#include <atmega32_board_driver.h>
#include <libhcan++/traceable_error.h>

namespace hcan
{
	class usv_error : public traceable_error
	{
		public:
			usv_error(const string &);
	};

	class usv_driver : public atmega32_board_driver 
	{
		private:
			void print_U_stats(uint16_t n, const string &name);
		protected:
			virtual bool show_help ();
		public:
			usv_driver (board_connection &bcon);
			virtual ~usv_driver();
			virtual bool exec_command (context &c, const string &);
	};

};

#endif
