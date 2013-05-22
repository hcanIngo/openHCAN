#include <iostream>
#include <driver_factory.h>

#include <board_driver.h>
#include <atmega8_board_driver.h>
#include <atmega32_board_driver.h>
#include <controller1612_driver.h>
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
	switch (arch)
	{
		case HCAN_ARCH_ATMEGA8 :
			{
				switch (type)
				{
					case 1 :
						return board_driver_p(new atmega8_board_driver(bcon, 64));
					default : 
						cerr << "warning: unknown board type id " << int(type)
							<< ", using generic atmega8 driver..." << endl;
						return board_driver_p(new atmega8_board_driver(bcon, 64));
				};
			}

		case HCAN_ARCH_ATMEGA32 :
			{
				switch (type)
				{
					case 4 : // Controllerboard-1612 v01
						return board_driver_p(new controller1612_driver(bcon));
					case 5 : // Userpanel v01
						return board_driver_p(new userpanel_driver(bcon));
					case 6 : // USV Controller1612-Module
						return board_driver_p(new usv_driver(bcon));
					case 7 : // Heizungssteuerung Controllerboard-1612 v01
						return board_driver_p(new controller1612_driver(bcon));
					case 8 : // Wetterstation Controllerboard-1612 v01
						return board_driver_p(new controller1612_driver(bcon));
					default : 
						cerr << "warning: unknown board type id " << int(type)
							<< ", using generic atmega32 driver..." << endl;
						return board_driver_p(new atmega32_board_driver(bcon));
				};
			}

		case HCAN_ARCH_ATMEGA644 :
			{
				switch (type)
				{
					case 4 : // Controllerboard-1612 v01
						return board_driver_p(new controller1612_driver(bcon, 256)); //auto-ptr-Objekt: Initialisierung nicht durch Zuweisung, sondern in Konstruktorschreibweise
					case 5 : // Userpanel v01
						return board_driver_p(new userpanel_driver(bcon, 256));
					default :
						cerr << "warning: unknown board type id " << int(type)
							<< ", using generic atmega644 driver..." << endl;
						return board_driver_p(new atmega32_board_driver(bcon, 256));
				};
			}

		default : // unknown arch
			cerr << "warning: unknown arch id " << int(arch)
				<< ", using generic hcan board driver..." << endl;
			return board_driver_p(new board_driver(bcon));
	};

	// never reached
	return board_driver_p();
}

