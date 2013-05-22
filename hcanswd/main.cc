#include "main.h"
#include <libhcan++/transport_connection.h>
#include <libhcandata/data_file_writer.h>

#include <boost/program_options.hpp>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>

namespace po = boost::program_options;

using namespace hcan;

volatile bool signal_quit = false;
volatile bool signal_reload = false;

void sighandler(int signal)
{
	switch (signal)
	{
		case SIGHUP :
			signal_reload = true;
			break;
		case SIGINT :
		case SIGTERM :
			signal_quit = true;
			break;
	}
}

void run_standard_mode (const string &filename)
{
		transport_connection tp(inet_addr("127.0.0.1"));
		data_file_writer dfw(filename);
		while (! signal_quit)
		{
			const frame f = frame::read_from(tp.socket());
			tp.keep_connection_alive();

			uint8_t data[8];
			for (int i = 0; i < 8; i++)
				data[i] = f.data(i);

			dfw.write_frame(f.src(), f.dst(), f.proto(), f.size(), data);
		}
		cerr << "terminating..." << endl;
}

/** 
 * generiert den Filenamen anhand des Datums
 */
string generate_filename()
{
	time_t now = time(0);
	struct tm *t = localtime(&now);
	
	char s[32];
	snprintf(s,sizeof(s)-1,"%d%02d%02d", 
			t->tm_year + 1900, 
			t->tm_mon + 1, 
			t->tm_mday);

	return string(s) + ".hdump";
}

void run_archive_mode(const string &dir)
{
	while (! signal_quit)
	{
		transport_connection tp(inet_addr("127.0.0.1"));

		const string filename = generate_filename();

		data_file_writer dfw(dir + "/" + filename);
		while (! (signal_quit || signal_reload))
		{
			const frame f = frame::read_from(tp.socket());
			tp.keep_connection_alive();

			uint8_t data[8];
			for (int i = 0; i < 8; i++)
				data[i] = f.data(i);

			dfw.write_frame(f.src(), f.dst(), f.proto(), f.size(), data);

			// Pruefen, ob es Mitternacht ist, und das Logfile rotiert
			// werden muss:
			if (generate_filename() != filename)
				break;
		}

		// Zuruecksetzen, damit beim naechsten Durchlauf nicht gleich wieder
		// reloaded wird
		signal_reload = false;
	}
}

void handle_given_options (const po::parsed_options &options,
        const po::options_description &option_desc,
        const po::variables_map &map)
{
    if (options.options.size() == 0 || map.count("help"))
    {
        cerr << option_desc << "\n";
        exit(1);
    }

    if (map.count("user"))
    {
		struct passwd *pw = getpwnam(map["user"].as<string>().c_str());

		if (pw == 0)
			throw traceable_error("unknown user");

		cerr << "switching to user " << map["user"].as<string>() << endl;

		if (setuid(pw->pw_uid) == -1)
			throw traceable_error("setuid() Problem");
    }

	if (map.count("archive-mode"))
	{
		if (map.count("dir"))
		{
			run_archive_mode(map["dir"].as<string>());
		}
		else
			throw traceable_error("--dir missing");

	}
	else
	{
		if (map.count("out"))
		{
			run_standard_mode(map["out"].as<string>());
		}
		else
			throw traceable_error("--out missing");
	}
}


int main(int argc, char *argv[])
{
	try
	{

		signal(SIGHUP, sighandler);
		signal(SIGINT, sighandler);
		signal(SIGTERM, sighandler);

		po::options_description options("hcanswd options");

		options.add_options()
			("help,h", "shows the available options")
			("user", po::value<string>(), "user id to run under")
			("archive-mode", "Archive mode logs to directory ")
			("out,o", po::value<string>(), "standard mode: file to store data to")
			("dir", po::value<string>(), "archive mode: directory to store data to")
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
		cerr << e.what() << endl;
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

