#include "main.h"
#include <libhcandata/data_file.h>
#include <libhcandata/data_query.h>
#include <libhcan++/traceable_error.h>
#include <libhcan++/frame.h>
#include <boost/regex.hpp>
#include <iostream>
#include <iomanip>
#include <stack>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace hcan;

// Globale Option: n Frames von hinten query'en
int last = 0;
bool numeric = false;
bool color = true;
bool interpret = false;

void print_frame(data_file_frame_entry &fe)
{
	frame f(fe.src, fe.dst, fe.proto, fe.data, fe.size);

	char ts[32];
	time_t tt = fe.timestamp;
	strftime(ts,sizeof(ts),"%c ",localtime(&tt)); // was: %X

	if (interpret && (fe.proto == HCAN_PROTO_SFP) &&
			(fe.data[0] == HCAN_SRV_HES) && 
			(fe.data[1] == HCAN_HES_1WIRE_TEMPERATURE))
	{
		float temp;
		temp = (int16_t)((fe.data[3]<< 8)|fe.data[4]) / 16.0;
		cout << ts << " tempsensor: " << (uint16_t)fe.data[2] << " temp: " 
			<< temp << std::endl;
	}
	else
		f.print(numeric,color,string(ts));
}

void query_file(const string &file, 
		const string &query_expression,
		int last)
{
	init_frame_message_description();

	data_file df(file);
	data_query dq(query_expression);

	if (last)
	{
		stack<data_file_frame_entry> st;

		// 1. von hinten die letzten n Matches in einen Stack
		// schieben
		int n = 0;
		for (int i = df.count() - 1; i >= 0; i--)
		{
			if (n >= last)
				break;

			data_file_frame_entry fe = df.frame(i);
			if (dq.match(fe))
			{
				st.push(fe);
				n++;
			}
		}

		// 2. den Stack ausgeben; dadurch dreht sich die Reihenfolge
		// um
		//
		while (st.size())
		{
			print_frame(st.top());
			st.pop();
		}
	}
	else
	{
		for (int i = 0; i < df.count(); i++)
		{
			data_file_frame_entry fe = df.frame(i);

			if (dq.match(fe))
				print_frame(fe);
		}
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

	string query_expression = "1";
	if (map.count("query"))
	{
		query_expression = map["query"].as<string>();
	}

	if (map.count("interpret"))
		interpret = true;

	if (map.count("numeric"))
		numeric = true;

	if (map.count("nocolor"))
		color = false;
	
	if (map.count("in"))
	{
		int last = 0;
		if (map.count("last"))
			last = map["last"].as<int>();

		query_file(map["in"].as<string>(), 
				query_expression,
				last);
	}
	else
		throw traceable_error("no input file given");
	
}

int main(int argc, char *argv[])
{
	po::options_description options("hcandq options");

	options.add_options()
		("help,h", "shows the available options")
		("in,i", po::value<string>(), "standard mode: file to query")
		("query,q", po::value<string>(), "query expression")
		("last,l", po::value<int>(), "show last n frames of query")
		("numeric,n", "numeric output")
		("nocolor", "no color")
		("interpret,I", "interpret frames")
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

