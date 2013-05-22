#ifndef DRIVER_FACTORY_H
#define DRIVER_FACTORY_H

#include <inttypes.h>
#include <memory>
#include <libhcan++/traceable_error.h>
#include <board_driver.h>

using namespace std;


namespace hcan
{
	class driver_error : public traceable_error
	{
		public:
			driver_error (const string &s);
	};

	enum { HCAN_ARCH_ATMEGA8 = 0, HCAN_ARCH_ATMEGA32 = 1, HCAN_ARCH_ATMEGA644 = 2,
			HCAN_ARCH_AUTODETECT = 255 };

	class driver_factory 
	{
		private:
		public:
			driver_factory () {};
			virtual ~driver_factory () {};
			board_driver_p make_driver (board_connection &bcon,
					const uint8_t arch, const uint8_t type) const;
	};

};

#endif
