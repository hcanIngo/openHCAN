#include <iostream>
#include <driver_factory.h>

#include <board_driver.h>
#include <atmega_board_driver.h>
#include <controllerboard_driver.h>
#include <userpanel_driver.h>
#include <usv_driver.h>

using namespace std;
using namespace hcan;

driver_error::driver_error (const string &s) :
	traceable_error(s)
{
}

board_driver_p driver_factory::make_driver (board_connection &bcon,
		const uint8_t arch, const uint8_t type) const
{
	uint16_t page_size=128; // default
	switch (arch)
	{
		case HCAN_ARCH_ATMEGA32 :
		case HCAN_ARCH_ATMEGA32_legacy :
			switch (type)
			{
				case HCAN_BOARDTYP_C1612 : // Controllerboard-1612 v01
					return board_driver_p(new controllerboard_driver(bcon));
				case HCAN_BOARDTYP_UI : // Userpanel v01
					return board_driver_p(new userpanel_driver(bcon));
				case HCAN_BOARDTYP_USV : // USV Controller1612-Module
					return board_driver_p(new usv_driver(bcon));
				case HCAN_BOARDTYP_ZENTRALHEIZUNG : // Heizungssteuerung Controllerboard-1612 v01
					return board_driver_p(new controllerboard_driver(bcon));
				case HCAN_BOARDTYP_WETTERSTATION : // Wetterstation Controllerboard-1612 v01
					return board_driver_p(new controllerboard_driver(bcon));
				default :
					cerr << "warning: unknown board type id " << int(type)
						<< ", using generic atmega32 driver..." << endl;
					return board_driver_p(new atmega_board_driver(bcon));
			};

			case HCAN_ARCH_ATMEGA644P :
			case HCAN_ARCH_ATMEGA644P_legacy :
				page_size = 256;
				switch (type)
				{
					case HCAN_BOARDTYP_C1612 : // Controllerboard-1612 v01
						return board_driver_p(new controllerboard_driver(bcon, page_size));
					case HCAN_BOARDTYP_UI : // Userpanel v01
						return board_driver_p(new userpanel_driver(bcon, page_size));
					default :
						cerr << "warning: unknown board type id " << int(type)
							<< ", using generic atmega644p driver..." << endl;
						return board_driver_p(new atmega_board_driver(bcon, page_size));
				};

				case HCAN_ARCH_ATMEGA328P :
					switch (type)
					{
						case HCAN_BOARDTYP_C1416 :
							return board_driver_p(new controllerboard_driver(bcon, page_size));
						default :
							cerr << "warning: unknown board type id " << int(type)
								<< ", using generic atmega328p driver..." << endl;
							return board_driver_p(new atmega_board_driver(bcon));
					};

		default : // unknown arch
			cerr << "warning: unknown arch id " << std::hex << int(arch)
				<< ", using generic hcan board driver..." << endl;
			return board_driver_p(new board_driver(bcon));
	};

	// never reached
	return board_driver_p();
}

