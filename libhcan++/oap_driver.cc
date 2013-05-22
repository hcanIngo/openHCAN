#include <oap_driver.h>
#include <oap-def.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <eds_commands.h>
#include <assert.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <inttypes.h>

#include <libhcan++/oap_access.h>
#include <libhcan++/oap_desc.h>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace hcan;
using namespace std;
using namespace boost;

oap_error::oap_error(const string &s) :
	traceable_error(s)
{
}

oap_driver::oap_driver (board_connection &bcon) :
	m_oap(bcon)
{

}

oap_driver::~oap_driver()
{
}

bool oap_driver::show_help ()
{
	cout << 
		"	oap walk\n" <<

		endl;

	return true;
}

string field_to_path(const oap_object &o, const oap_field &f, uint8_t idx = 0)
{
	// Pfad: {tag_nacht_agent}4.tag_active[12]
	stringstream ss;
	ss << "{" << o.class_name() <<
		"}" << (int)o.oid() << "." << f.name();

	// In Abhaengigkeit davon, ob es ein normales Feld oder
	// ein array ist, ggfs. einen Index dahinterschreiben:
	if (f.arraysize() > 0)
	{
		ss << '[' << (int)idx << ']';
	}

	return ss.str();
}

void oap_driver::cmd_walk()
{
	for (uint8_t oid = 0; oid < m_oap.object_count(); oid++)
	{
		oap_object o = m_oap.object(oid);

		for (uint8_t fid = 0; fid < o.field_count(); fid++)
		{
			oap_field f = o.field(fid);

			uint8_t arraysize = f.arraysize();
			if (arraysize > 0)
			{
				for (uint8_t idx = 0; idx < arraysize; idx++)
				{
					cout << field_to_path(o,f,idx) << " : " <<
						f.value_as_string(idx) << endl;

					//path2field(field_to_path(o,f,idx));
				}
			}
			else
			{
					cout << field_to_path(o,f) << " : " <<
						f.value_as_string() << endl;
			}
		}
	}
}

bool oap_driver::exec_command (context &c, const string &command)
{
	istringstream sin(command);

	string s;
	sin >> s;

	if (s == "oap")
	{
		sin >> s;

		if (s == "walk")
		{
			cmd_walk();
			return true;
		}

		if (s == "edit")
		{
			uint16_t idx;
			sin >> idx;

			if (idx <= m_oap.object_count()-1)
			{
				oap_object o = m_oap.object(idx);

				stringstream ss;
				ss << "OAP/{" << o.class_name() << "}" << idx << " > ";
				c.prompt = ss.str();
				c.mode = context::oap;
				c.oap_oid = idx;
				return true;
			}
			else
			{
				cout << "error: oap object id out of range" << endl;
				return true;
			}
		}

		if (s == "list")
		{
			for (int i = 0; i < m_oap.object_count(); i++)
			{
				oap_object o = m_oap.object(i);
				cout << "{" << o.class_name() << "}" << i << endl;
			}

			return true;
		}
	}

	if ((s == "exit") && (c.mode == context::oap))
	{
		c.reset();
		return true;
	}

	if ((s == "list") && (c.mode == context::oap))
	{
		oap_object o = m_oap.object(c.oap_oid);
		for (int i = 0; i < o.field_count(); i++)
		{
			oap_field f = o.field(i);
			if (f.arraysize())
			{
				for (int idx = 0; idx < f.arraysize(); idx++)
				{
					cout << "{" << f.class_as_string() << "}" << 
						f.name() << "[" << idx << "] : " << 
						f.value_as_string(idx) << endl;
				}
			}
			else
			{
				cout << "{" << f.class_as_string() << "}" << 
					f.name() << " : " << f.value_as_string() << endl;
			}
		}

		return true;
	}

	if ((s == "set") && (c.mode == context::oap))
	{
		string key;
		string value;
		sin >> key >> value;

		oap_object o = m_oap.object(c.oap_oid);

		try
		{
			boost::regex re("([a-zA-Z][_a-zA-Z0-9]*)(\\[(\\d+)\\])?");
			boost::cmatch matches;

			if (boost::regex_match(key.c_str(), matches, re))
			{
				string fieldname = matches[1];

				if (matches[3] == "") // Feld ohne Index
				{
					oap_field f = o.field_by_name(fieldname);
					f.set_value(value);
					return true;
				}
				else // Feld mit Index
				{
					oap_field f = o.field_by_name(fieldname);
					uint16_t idx = lexical_cast<int>(matches[3]);
					if (idx <= f.arraysize()-1)
					{
						f.set_value(value,idx);
						return true;
					}
					else
						throw traceable_error("field index out of range");
				}
			}
			else
				throw traceable_error("format error, regex did not match");
		}
		catch (traceable_error &e)
		{
			cout << "error in field format: " << e.what() << endl;
		}
	}

	return false;
}

