#ifndef HAVE_CONFIG_H
//#error "config.h missing"
#endif

//#include <config.h>
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
#include <readlinepp.h>
#include <control.h>
#include <libhcan++/colors.h>
#include <hcan_multicast.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef __WIN32__
#define sleep(a) _sleep(a)
#endif

#define HISTFILE ".telican-history"

namespace po = boost::program_options;

using namespace std;

in_addr_t hcand_ip;

vector<string> parse_command_list(const string &commands)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(";");
	tokenizer tokens(commands, sep);
	vector<string> result;

	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter)
	{
		string command = *tok_iter;
		boost::trim(command);
		result.push_back(command);
	}

	return result;
}

void run_templog_mode(const po::variables_map &map)
{
	hcan::transport_connection con(hcand_ip);
	const bool resolve = map.count("resolve");
	con.templog(resolve);
	exit(0);
}

void run_syslog_mode(const po::variables_map &map)
{
	hcan::transport_connection con(hcand_ip);
	const bool resolve = map.count("resolve");
	con.syslog(resolve);
	exit(0);
}

void run_timed_mode(const po::variables_map &map)
{
	uint16_t src = 0;
	src = src; // to avoid warning "-Werror=unused-but-set-variable"
	if (map.count("src"))
		src = map["src"].as<uint16_t>();

	hcan::transport_connection con(hcand_ip);

	int last_minute = 0;

	while (1)
	{
		const time_t now = time(0);
		const struct tm *tm = localtime(&now);

		if (tm->tm_min != last_minute)
		{
			last_minute = tm->tm_min;

			int day_of_week = tm->tm_wday == 0 ? 7 : tm->tm_wday;

			// Level is 0, because we are the master
			con.send_TIME_INFO(
					HCAN_MULTICAST_INFO, HCAN_MULTICAST_INFO,
					0, 
					day_of_week,
					tm->tm_hour,
					tm->tm_min,
					tm->tm_sec);
			sleep(1);

			// Hint: tm_year means (see localtime(3)):
			//       The number of years since 1900
			//       So we have to adopt it to 2000:
			//
			//       Example: 
			//       - it is 2005
			//       - tm_year would be 105
			//       - our year counter has to be 5
			//       - so we have to subtract 100 from tm_year
			//       - this is done below
			//
			con.send_DATE_INFO(
					HCAN_MULTICAST_INFO, HCAN_MULTICAST_INFO,
					0, 
					tm->tm_mday,    // localtime() gives [1..31]
					tm->tm_mon + 1, // localtime() gives [0..11]
					tm->tm_year - 100);
		}
		sleep(1);
	}
	exit(0);
}

/**
Modi:

--help
--timed
--templog
--syslog
--dump
--dump-no-syslog
--ping
--floodping
--pingonce
--connect
--control // Default, wenn nichts angegeben ist

*/

void handle_given_options (const po::parsed_options &options, 
		const po::options_description &option_desc,
		const po::variables_map &map)
{
	/** if (options.options.size() == 0 || map.count("help")) */

	if (map.count("resolve"))
	{
		if (!map.count("templog") && !map.count("dump") && !map.count("dump-no-syslog") && !map.count("syslog"))
		{
			cerr << "resolve only works with \"--templog\", \"--syslog\", \"--dump\" or \"--dump-no-syslog\"" << "\n";
			exit(1);
		}
	}

	if (map.count("help"))
	{
		cerr << option_desc << "\n";
		exit(1);
	}

	if (getenv("HCAND_ADDRESS"))
	{
		const string hcand_address_env(getenv("HCAND_ADDRESS"));
		if (hcand_address_env != "")
			hcand_ip = inet_addr(hcand_address_env.c_str());
	}

	if (map.count("ip-address"))
	{
		const string ip_address(map["ip-address"].as<string>());
		hcand_ip = inet_addr(ip_address.c_str());
		cerr << "IP address : " << ip_address << endl;
		if (hcand_ip == 0)
		{
			cerr << "IP address error: " << ip_address << endl;
			exit(1);
		}
	}

	if (map.count("timed"))
		run_timed_mode(map);

	if (map.count("templog"))
		run_templog_mode(map);

	if (map.count("syslog"))
		run_syslog_mode(map);

	// Dump mode; just open a transport connection an
	// dump everything we see
	if (map.count("dump"))
	{
		hcan::transport_connection con(hcand_ip);
		const bool numeric = map.count("numeric");
		const bool nocolor = map.count("nocolor");
		const bool resolve = map.count("resolve");
		con.dump(numeric, !nocolor, true, resolve);
		exit(0);
	}
	if (map.count("dump-no-syslog"))
	{
		hcan::transport_connection con(hcand_ip);
		const bool numeric = map.count("numeric");
		const bool nocolor = map.count("nocolor");
		const bool resolve = map.count("resolve");
		con.dump(numeric, !nocolor, false, resolve);
		exit(0);
	}

	// Standard NonPolite
	if (map.count("polite-time"))
	{
		hcan::frame::polite_mode = true;
		hcan::frame::polite_wait_time = 
			map["polite-time"].as<uint16_t>() * 1000;
	}
	else
		hcan::frame::polite_mode = false;

	if (map.count("ping"))
	{
		// bei ping muss die Zeit gemessen werden; hier keinen Polite
		// Mode aktivieren!
		hcan::frame::polite_mode = false;

		// setup the destination address of our peer board
		uint16_t dst = map["ping"].as<uint16_t>();

		// 0 bedeutet: bei hcanaddressd eine Adresse reservieren
		uint16_t src = 0;

		hcan::dynaddress dynsrc(hcand_ip);

		if (map.count("src"))
			src = map["src"].as<uint16_t>();
		else
		{
			dynsrc.allocate();
			src = dynsrc();
		}

		hcan::transport_connection con(hcand_ip);

		con.ping(src, dst);
		exit(0);
	}

	if (map.count("floodping"))
	{
		// bei ping muss die Zeit gemessen werden; hier keinen Polite
		// Mode aktivieren!
		hcan::frame::polite_mode = false;

		// setup the destination address of our peer board
		uint16_t dst = map["floodping"].as<uint16_t>();

		// 0 bedeutet: bei hcanaddressd eine Adresse reservieren
		uint16_t src = 0;

		hcan::dynaddress dynsrc(hcand_ip);

		if (map.count("src"))
			src = map["src"].as<uint16_t>();
		else
		{
			dynsrc.allocate();
			src = dynsrc();
		}

		hcan::transport_connection con(hcand_ip);

		con.flood_ping(src, dst,100);
		exit(0);
	}

	if (map.count("pingonce"))
	{
		// bei ping muss die Zeit gemessen werden; hier keinen Polite
		// Mode aktivieren!
		hcan::frame::polite_mode = false;

		// setup the destination address of our peer board
		uint16_t dst = map["pingonce"].as<uint16_t>();

		// 0 bedeutet: bei hcanaddressd eine Adresse reservieren
		uint16_t src = 0;

		hcan::dynaddress dynsrc(hcand_ip);

		if (map.count("src"))
			src = map["src"].as<uint16_t>();
		else
		{
			dynsrc.allocate();
			src = dynsrc();
		}

		hcan::transport_connection con(hcand_ip);

		con.ping_once(src, dst);
		exit(0);
	}

	if (map.count("connect"))
	{
		// Ueberpruefen, ob keine inkompatiblen Modi angegeben wurden
		try
		{
			// setup the destination address of our peer board
			uint16_t dst = map["connect"].as<uint16_t>();
			// 0 bedeutet: bei hcanaddressd eine Adresse reservieren
			uint16_t src = 0;

			hcan::dynaddress dynsrc(hcand_ip);

			if (map.count("src"))
				src = map["src"].as<uint16_t>();
			else
			{
				dynsrc.allocate(); //ohne src-Angabe: hcanaddressd
				src = dynsrc();
			}

			hcan::transport_connection con(hcand_ip);
			bool ignore_type = map.count("ignore-type") > 0;

			uint8_t arch = hcan::HCAN_ARCH_AUTODETECT;

			if (map.count("arch"))
			{
				const string arch_name = map["arch"].as<string>();
				if (arch_name == "atmega32")
					arch = hcan::HCAN_ARCH_ATMEGA32;
				else if (arch_name == "atmega644p")
					arch = hcan::HCAN_ARCH_ATMEGA644P;
				else if (arch_name == "atmega328p")
					arch = hcan::HCAN_ARCH_ATMEGA328P;
				else
				{
					cerr << "unknown arch!\n";
					exit(1);
				}
			}

			hcan::board_connection bcon(con, src, dst, ignore_type, arch);
			// Als erstes schauen, ob Befehle aus einer Datei abzuarbeiten
			// sind:
			if (map.count("read"))
			{
				// ja, also Datei laden...
				const string filename = map["read"].as<string>();
				ifstream in(filename.c_str());

				if (! in)
				{
					cerr << "could not open file " << filename << endl;
					exit(1);
				}

				hcan::context c;

				char buf[512];
				int n = 1;
				while (in.getline(buf,sizeof(buf)))
				{
					if (! bcon.get_driver()->exec_command(c,string(buf)))
					{
						cerr << "syntax error in line " << n << ": " 
							<< string(buf) << endl;
						exit (1);
					}
					n++;
				}
			}

			if (map.count("incremental-flash"))
			{
				incremental_flash = true;
			}

			if (map.count("exec"))
			{
				// we got commands from the commandline args
				typedef vector<string> veclist;
				const veclist commands = parse_command_list(
						map["exec"].as<string>());

				hcan::context c;

				for (veclist::const_iterator i = commands.begin();
						i != commands.end(); i++)
				{
					if (! bcon.get_driver()->exec_command(c,*i))
					{
						cerr << "syntax error: " << *i << endl;
						exit (1);
					}
				}
			}
			else
			{
				// interactive mode; REPL
				char *home_env = getenv("HOME");
				string histfile = "/";

				if (home_env)
					histfile = string(home_env) + "/" + HISTFILE;

				readlinepp::history history(histfile);

				hcan::context c;
				while (1)
				{
					try
					{
#ifdef __WIN32__
						cout << COLOR_RED;
#else
						cout << COLOR_BLUE;
#endif
						cout.flush();
						const string line = readlinepp::read_line(c.prompt);

						if (line == "quit")
							throw readlinepp::end_of_input();

						cout << COLOR_NORMAL;
						cout.flush();

						if (! bcon.get_driver()->exec_command(c,line))
							cerr << "syntax error" << endl;
						else
						{
							if (home_env && (line != ""))
								history.add(line);
						}
					}
					catch (readlinepp::end_of_input &e)
					{
						cout << COLOR_NORMAL;
						cout << endl;

						if (c.mode == hcan::context::normal)
							break;

						if (c.mode == hcan::context::edit)
						{
							c.mode = hcan::context::normal;
							c.prompt = c.std_prompt;
						}
						if (c.mode == hcan::context::oap)
						{
							c.mode = hcan::context::normal;
							c.prompt = c.std_prompt;
						}
					}
				}
			}
		}
		catch (hcan::transport_error &e)
		{	
			cerr << "transport connection error: " << e << endl;
			exit(1);
		}
		return; // verlassen, bevor der Control-Mode betreten wird
	}

	// wenn kein Modus mehr uebrigbleibt, dann ist es
	// der Control-Modus

	control_mode(hcand_ip);
}

int main (int argc, char *argv[])
{
	hcand_ip = inet_addr("127.0.0.1");

	try
	{
		po::options_description options("telican options");

		options.add_options()
			("help,h", "shows the available options")
			("ip-address,a", po::value<string>(),  "IP adress of hcand (default 127.0.0.1)")
			("dump,d", "dump mode; dump all messages")
			("dump-no-syslog,D", "dump mode; dump all messages; exept of syslog messages")
			("nocolor", "no color in dump mode")
			("numeric,n", "in dump mode, print frame data as numbers")
			("resolve", "try to resolve group numbers and IDs from installation.xml")
			("ping,p", po::value<uint16_t>(), 
			 "sends ping frames to the given destination")
			("floodping,P", po::value<uint16_t>(), 
			 "sends flood pings to the given destination")
			("pingonce", po::value<uint16_t>(),
			 "sends one ping to the given destination")
			("connect,c", po::value<uint16_t>(), "connect to given destination")
			("ignore-type", "ignore the remote type if device does not answer")
			("arch", po::value<string>(), "set arch: [atmega328p|atmega32|atmega644p] ")
			("exec,e", po::value<string>(), "execute the given command(s)")
			("src,s", po::value<uint16_t>(), "use given hcan source address")
			("read,r", po::value<string>(), "read commands from given file")
			("timed", "time daemon mode; broadcasts time and date")
			("templog", "temp log mode; logs all sniffed temps")
			("syslog", "syslog mode; logs all syslog messages on the bus")
			("polite-time", po::value<uint16_t>(),
			 "set polite delay between frames (msec)")
			("incremental-flash,C","experimental: reduce flash time")
			;

		po::options_description config_file_options;
		config_file_options.add(options);

		po::variables_map option_variable_map;

		po::parsed_options p_options = po::parse_command_line(
				argc, argv, options);

		po::store(p_options, option_variable_map);
		po::notify(option_variable_map);    

		handle_given_options(p_options, options,  option_variable_map);

		exit(0);
	}
	catch (po::error &e)
	{
		cerr << e.what() << endl;
	}
	catch (hcan::dynaddress_error &e)
	{
		cerr << e.what() << endl;
		cerr << "HINT: use -s to set a source address\n";
	}
	catch (traceable_error &e)
	{
		cerr << endl;
		for (vector<string>::const_iterator i = e.trace().begin();
				i != e.trace().end(); i++)
		{
			cerr << "  " << *i << endl;
		}
		cerr << endl;
		cerr << e << endl;
	}
	catch (std::exception &e)
	{
		cerr << "std::exception error: " << e.what() << endl;
	}
	catch (...)
	{
		cerr << "unknown error caught." << endl;
	}
	exit(1);
}

