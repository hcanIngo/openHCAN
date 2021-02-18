#include <userpanel_driver.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <assert.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <inttypes.h>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h> // for STDOUT_FILENO

#include <eds_desc.h>
#include <eds_connection.h>

using namespace hcan;
using namespace std;

namespace hcan
{

void eds_cmd_list(board_connection &bcon, eds_connection &econ, context &c, uint16_t address)
{
	try
	{
		if (c.mode == context::normal || address != 0)
		{
			for (eds_connection::eds_blocks::const_iterator i =
					econ.blocks().begin(); i != econ.blocks().end(); i++)
			{
				if(address != 0 && i->address() != address) continue;

				cout << "{" << i->type_name() << "}@" << i->address() << endl;
			}
		}

		if (c.mode == context::edit || address != 0)
		{
			uint16_t adressToList;
			if(address == 0) adressToList = c.eds_block_address;
			else adressToList = address;

			eds_block &block = econ.block_by_address(adressToList);

			for (eds_block_fields_t::const_iterator i =
					block.fields().begin(); i != block.fields().end(); i++)
			{
				if(address != 0 && adressToList != address) continue;

				if (i->datatype == "char")
				{
					cout << "char[" << i->size << "] " << i->name
						<< " '" << block.strfield(i->name) << "'" << endl;
				}
				else if (i->datatype == "uint16_t")
				{
					cout << i->datatype << " " << i->name
						<< " " << block.uint16_field(i->name) << endl;
				}
				else
				{
					cout << i->datatype << " " << i->name
						<< " " << (uint16_t)block.field(i->name) << endl;
				}
			}
		}
	}
	catch (traceable_error &e)
	{
		cerr << e.what() << endl;
	}
}

void eds_cmd_field_hint(board_connection &bcon, eds_connection &econ,
		context &c, const string &field)
{
	uint16_t adressToList = c.eds_block_address;

	eds_block &block = econ.block_by_address(adressToList);

	for (eds_block_fields_t::const_iterator i =
			block.fields().begin(); i != block.fields().end(); i++)
	{
		if(i->name != field) continue;
		if(i->bits.size() != 0)
		{
			signed vorlauf = 6;
			vorlauf += block.type_name().length();
			vorlauf += (std::to_string(c.eds_block_address)).length();

			struct winsize size;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
			signed windowWidth = size.ws_col;
			signed noOfBits = i->bits.size()-1;

			for (signed j = noOfBits; j >= 0; j--)
			{
				cout << "# bit " << j << "   ";

				for (signed k = 0; k < vorlauf; k++) cout << " ";
				for (unsigned k = 0; k < i->name.length(); k++) cout << " ";
				for (signed k = 0; k < 8-noOfBits; k++) cout << " ";
				for (signed k = noOfBits; k >= j; k--) cout << "|";
				for (signed k = 0; k < 2+j; k++) cout << "-";

				std::string description = (std::string)i->bits[j].description;
				if(description.length() >= (unsigned)(windowWidth-28))
				{
					description = description.substr (0,windowWidth-29-3) + "...";
				}
				cout << " " << description << endl;
			}

			cout << "# " << i->datatype;
			for (signed k = 0; k < vorlauf-1; k++) cout << " ";
			cout << i->name << " ";
			uint16_t no = (uint16_t)block.field(i->name);
			std::string binary = "0b";
			for(int i = 7; i >= 0; i--)
			{
				if(no & (1<<i)) binary += "1";
				else binary += "0";
			}
			cout << binary << " (" << (uint16_t)block.field(i->name) << ")" << endl;
		}
	}
}

void eds_cmd_create(board_connection &bcon, eds_connection &econ, 
		context &c, const string &type)
{
	try
	{
		eds_block &block = econ.create_block(type);

		cout << "creating new " << type
			<< ", type id = " << (uint16_t)(block.type()) << "..." << endl;

		stringstream ss;
		ss << "EDS/{" << block.type_name() << "}@" << block.address() << " > ";

		c.prompt = ss.str();
		c.mode = context::edit;
		c.eds_block_address = block.address();

		return;
	}
	catch (traceable_error &e)
	{
		cerr << e.what() << endl;
	}
}

void eds_cmd_delete(board_connection &bcon, eds_connection &econ, 
		context &c, uint16_t address)
{
	try
	{
		econ.delete_block(econ.block_by_address(address));
	}
	catch (traceable_error &e)
	{
		cerr << e.what() << endl;
	}
}

void eds_cmd_edit(board_connection &bcon, eds_connection &econ, context &c, 
		uint16_t address)
{
	try
	{
		const eds_block &block = econ.block_by_address(address);
		stringstream ss;
		ss << "EDS/{" << block.type_name() << "}@" << address << " > ";

		c.prompt = ss.str();
		c.mode = context::edit;
		c.eds_block_address = address;
	}
	catch (traceable_error &e)
	{
		cerr << e.what() << endl;
	}
}


void eds_cmd_set_field(board_connection &bcon, eds_connection &econ,
		context &c, uint16_t address, 
		const string &key, const string &value)
{
	try
	{
		eds_block &block = econ.block_by_address(address);
		string field_datatype = block.field_datatype(key);

		if (field_datatype == "char")
		{
			block.set_field_str(key,value);
		}
		else if (field_datatype == "uint16_t")
		{
			istringstream ss(value);
			int v;
			ss >> v;
			block.set_field_uint16(key, (uint16_t) v);
		}
		else if (field_datatype == "uint8_t")
		{
			istringstream ss(value);
			int v;
			if(value.rfind("0b", 0) == 0 && value.length() == 10)
			{
				string bitvalue = value.substr (2,8);
				v = 0;

				for(int i = 0; i < 8; i++)
				{
					if (bitvalue.substr (7-i,1)  == "1")
					{
						v |= (1<<i);
					}
					else if (bitvalue.substr (7-i,1)  == "0")
					{
						v &= ~(1<<i);
					}
					else
					{
						cout << "Error: bei binaerer Eingaben ist nur 0 und 1 erlaubt!" << endl;
						return;
					}
				}
			}
			else if ((value.rfind("0b", 0) == 0 && value.length() != 10) ||
					value.find("b") != std::string::npos || value.find("B") != std::string::npos)
			{
				cout << "Error: Binaere Eingaben wird als 0b00000000 erwartet!" << endl;
				return;
			}
			else
			{
				 ss >> v;
			}
			block.set_field_uint8(key, (uint8_t) v);
		}
		else
		{
			cout << "unknown eds field type defined in eds.xml: " << field_datatype << endl;
		}
	}
	catch (traceable_error &e)
	{
		cerr << e.what() << endl;
	}
}

void eds_cmd_show_conf(board_connection &bcon, eds_connection &econ, 
		context &c, const string &type)
{
	for (eds_connection::eds_blocks::iterator i = 
			econ.blocks().begin(); i != econ.blocks().end(); i++)
	{
		if (type != "" && i->type_name() != type) continue;
		cout << "# EDS-Address: " << i->address() << endl;
		cout << "create " << i->type_name() << endl;

		for (eds_block_fields_t::const_iterator j =
				i->fields().begin(); j != i->fields().end(); j++)
		{
			if (j->datatype == "char")
				cout << "  set " << j->name << " " << 
					i->strfield(j->name) << endl;
			else if (j->datatype == "uint16_t")
				cout << "  set " << j->name << " " << 
					i->uint16_field(j->name) << endl;
			else
				cout << "  set " << j->name << " " << 
					(uint16_t)i->field(j->name) << endl;
		}

		cout << "exit" << endl;
	}
}

bool eds_exec_cmd(board_connection &bcon, eds_connection &econ,
		context &c, const string &cmd)
{
	istringstream sin(cmd);

	string s;
	sin >> s;

	if (s == "show")
	{
		sin >> s;

		if (s == "conf")
		{
			eds_cmd_show_conf(bcon, econ, c, "");
			return true;
		}
		else if (s == "all")
		{
			sin >> s;
			eds_cmd_show_conf(bcon, econ, c, s);
			return true;
		}
	}
	if ((s == "list") && 
			((c.mode == context::normal) || (c.mode == context::edit)))
	{
		eds_cmd_list(bcon, econ, c, 0);
		return true;
	}

	if (s == "edit")
	{
		uint16_t address;
		sin >> address;

		eds_cmd_edit(bcon, econ, c,address);
		return true;
	}
	
	if (s == "print")
	{
		uint16_t address;
		sin >> address;

		eds_cmd_list(bcon, econ, c, address);
		return true;
	}

	if (s == "create")
	{
		sin >> s;

		eds_cmd_create(bcon, econ, c, s);
		return true;
	}

	if (s == "delete")
	{
		uint16_t address;
		sin >> address;

		eds_cmd_delete(bcon, econ, c, address);
		return true;
	}

	if (s == "defragment")
	{
		econ.defragment();
		econ.update();
		return true;
	}
	if (s == "Format")
	{
		bcon.tcon().send_FORMAT(bcon.src(), bcon.dst());
		econ.update();
		return true;
	}

	if ((s == "exit") && (c.mode == context::edit))
	{
		c.reset();
		return true;
	}

	if ((s == "hint") && (c.mode == context::edit))
	{
		sin >> s;

		eds_cmd_field_hint(bcon, econ, c, s);
		return true;
	}

	if (c.mode == context::edit)
	{
		if (s == "set")
		{
			string value;
			sin >> s;
			
			value = "";
			int i = 0;
			while (! sin.eof())
			{
				if (i > 0)
					value = value + " ";
				string s;
				sin >> s;
				value = value + s;
				i++;
			}
			eds_cmd_set_field(bcon, econ, c, c.eds_block_address, s, value);
			return true;
		}
	}

	return false;
}

bool eds_show_help ()
{
	cout << 
		"	list                  EDS: Zeigt alle Blocks an\n" <<
		"	print <n>             EDS: Zeigt die Felder des Blocks <n> an\n" <<
		"	edit <n>              EDS: Editiert den Block <n>\n" <<
		"	set <field> <v>       EDS: Setzt das Feld <field> auf den Wert <v>\n" <<
		"	hint <field>          EDS: Gibt erleuternde Infos fuer das Feld <field> aus, soweit vorhanden\n" <<
		"	create <blocktype>    EDS: Legt einen neuen Block an\n" <<
		"	delete <n>            EDS: Loescht den mit <n> spez. Block\n" <<
		"	defragment            EDS: Defragmentiert das EEPROM\n" <<
		"	Format                EDS: (!) Formatiert das EEPROM (!) \n" <<
		"	show conf             EDS: Zeigt alle Bloecke an \n" <<
		"	show all <x>          EDS: Zeigt alle Bloecke vom Typ <x> an  n" <<
		
	endl;

	return true;
}

};
