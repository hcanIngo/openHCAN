#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <libhcan++/traceable_error.h>
#include <libhcan++/transport_connection.h>
#include <libhcan++/board_connection.h>
#include <libhcan++/board_driver.h>
#include <libhcan++/driver_factory.h>
#include <libhcan++/dynaddress.h>
#include <libhcan++/colors.h>
#include <hcan_multicast.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class check_failed : public std::exception
{
};


namespace po = boost::program_options;

using namespace std;

in_addr_t hcand_ip;

void ping(hcan::transport_connection &tp, uint16_t src, uint16_t dst)
{
	tp.send_PING_REQUEST(src,dst);

	try
	{
		tp.recv_PING_REPLAY(dst,src);
		cout << "OK: Adresse " << dst << " erreichbar" << endl;
	}
	catch (hcan::transport_error &e)
	{
		cout << "CRITICAL: Adresse " << dst << " nicht erreichbar" << endl;
		throw check_failed();
	}
}

void usv_betrieb(hcan::transport_connection &tp, 
		uint16_t src, uint16_t dst)
{
	tp.send_STATE_REQUEST(src,dst);
	uint8_t state;
	tp.recv_STATE_INFO(dst,src,&state);

	if (state)
	{
		cout << "CRITICAL: USV auf Batteriebetrieb" << endl;
		throw check_failed();
	}
	else
		cout << "OK: USV auf Netzbetrieb" << endl;
}

void usv_min_bat_spannung(hcan::transport_connection &tp, 
		uint16_t src, uint16_t dst, float min)
{
	uint8_t dummy, min_hi, min_lo, max_hi, max_lo;

	// siehe usv_driver.cc
	tp.send_VOLTAGE_STATS_REQUEST(src, dst, 3); // 3 = Bat 1+2,
	tp.recv_VOLTAGE_STATS_REPLAY(dst, src,
			&dummy, &min_hi, &min_lo, &max_hi, &max_lo);

	float Umin = 30.0 / 1024.0 * (min_lo | (min_hi << 8));

	if (Umin < min)
	{
		cout << "CRITICAL: U(bat) = " << Umin << " V" << endl;
		throw check_failed();
	}
	else
	{
		cout << "OK: U(bat) = " << Umin << " V" << endl;
	}
}


void handle_given_options (const po::parsed_options &options, 
		const po::options_description &option_desc,
		const po::variables_map &map)
{
	if (options.options.size() == 0 || map.count("help"))
	{
		cout << option_desc << "\n";
		throw check_failed();
	}

	if (map.count("ip-address"))
	{
		const string ip_address(map["ip-address"].as<string>());
		hcand_ip = inet_addr(ip_address.c_str());
		if (hcand_ip == 0)
		{
			cout << "IP address error: " << ip_address << endl;
			throw check_failed();
		}
	}

	hcan::transport_connection con(hcand_ip);

	hcan::dynaddress dynsrc(hcand_ip);
	dynsrc.allocate();
	uint16_t src = dynsrc();

	uint16_t dst = 0;

	if (map.count("dst"))
	{
		dst = map["dst"].as<uint16_t>();
	}
	else
		throw po::error("no dst given");

	if (map.count("ping"))
	{
		ping(con,src,dst);
	}

	if (map.count("usv_betrieb"))
	{
		usv_betrieb(con,src,dst);
	}
		
	if (map.count("usv_min_bat_spannung"))
	{
		const float min = map["usv_min_bat_spannung"].as<float>();
		usv_min_bat_spannung(con,src,dst,min);
	}
}

int main (int argc, char *argv[])
{
	hcand_ip = inet_addr("127.0.0.1");

	try
	{
		po::options_description options("telican options");

		options.add_options()
			("help,h", "shows the available options")
			("ip-address,a", po::value<string>())
			("dst,d", po::value<uint16_t>(),  "dst address")
			("ping","pings the target")
			("usv_betrieb", "Prueft, ob Netz- oder Bat-Betrieb")
			("usv_min_bat_spannung", po::value<float>())
			;

		po::options_description config_file_options;
		config_file_options.add(options);

		po::variables_map option_variable_map;

		po::parsed_options p_options = po::parse_command_line(
				argc, argv, options);

		po::store(p_options, option_variable_map);
		po::notify(option_variable_map);    

		handle_given_options(p_options, options,  option_variable_map);

	}
	catch (po::error &e)
	{
		cout << e.what() << endl;
		exit(2);
	}
	catch (hcan::dynaddress_error &e)
	{
		cout << "CRITICAL: " << e.what() << endl;
		exit(2);
	}
	catch (check_failed &e)
	{
		// Message bereits beim Check ausgegeben, daher
		// direkt beenden:
		exit(2);
	}
	catch (traceable_error &e)
	{
		cout << "CRITICAL: " << e << endl;
		exit(2);
	}
	catch (std::exception &e)
	{
		cout << "CRITICAL: error: " << e.what() << endl;
		exit(2);
	}
	catch (...)
	{
		cout << "CRITICAL: unknown error caught." << endl;
		exit(2);
	}

	// Keine Exception, also OK
	exit(0);
}

