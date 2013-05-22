#include <eds_connection.h>
#include <board_connection.h>
#include <transport_connection.h>
#include <iostream>

using namespace std;
using namespace hcan;

/**
 * dieser Baum wird aus den Daten der XML Datei eds.xml erzeugt. Der
 * Baum beschreibt die im EDS Interface exisitenten Block-Typen und deren
 * Felder (Namen und Typen).
 */
const vector<eds_block_desc_t> eds_block_descriptions = 
	eds_get_block_descriptions();

// Hilfsfunktionen:
//

 
/**
 * Liefert das passende Block-Beschreibungsobjekt fuer die gegebene
 * Block-Typ-ID
 */
const eds_block_desc_t get_block_desc_by_type(uint8_t type)
{
	for (vector<eds_block_desc_t>::const_iterator i = 
			eds_block_descriptions.begin();
			i != eds_block_descriptions.end(); i++)
	{
		if (i->type == type)
		{
			return *i;
		}
	}

	eds_error e("eds block desc of type id ");
	throw e << type << " not found";
}

/**
 * Liefert das passende Field-Beschreibungsobjekt fuer den gegebenen
 * Key
 */
const eds_block_field_desc_t get_block_field_desc_by_key(
		const eds_block_desc_t &desc, const string &key)
{
	eds_block_fields_t::const_iterator i = desc.fields.begin();
	for (; i != desc.fields.end(); i++)
	{
		if (i->name == key)
			return *i;
	}

	eds_error e("eds block field desc of '");
	throw e << key << "' not found";
}

/**
 */
int get_block_field_index_by_key(const eds_block_desc_t &desc, 
		const string &key)
{
	eds_block_fields_t::const_iterator i = desc.fields.begin();
	for (int counter = 0; i != desc.fields.end(); i++, counter++)
	{
		if (i->name == key)
			return counter;
	}

	eds_error e("eds block field desc of '");
	throw e << key << "' not found";
}

/////////////////////////////////////////////////////////////////////////

eds_block::eds_block(eds_connection &econ, uint16_t address) :
	m_econ(econ),
	m_bcon(econ.bcon()),
	m_tcon(econ.bcon().tcon()),
	m_address(address),
	m_type(0),
	m_size(0),
	m_desc(init())
{
}

eds_block::eds_block(const eds_block &b) :
	m_econ(b.m_econ),
	m_bcon(m_econ.bcon()),
	m_tcon(m_bcon.tcon()),
	m_address(b.m_address),
	m_type(b.m_type),
	m_size(b.m_size),
	m_desc(b.m_desc)
{
}


const eds_block_desc_t eds_block::init()
{
	uint8_t d2, d3;
	m_tcon.send_READ_FROM_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address, 0);
	m_tcon.recv_READ_FROM_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
			&m_type, &m_size, &d2, &d3);

	return get_block_desc_by_type(m_type);
}

eds_block &eds_block::operator = (const eds_block &b)
{
	m_address = b.m_address;
	const_cast< eds_block_desc_t &>(m_desc) = init();
	return *this;
}

eds_block::~eds_block()
{
}

const string &eds_block::type_name() const
{
	return m_desc.name;
}

uint8_t eds_block::field(const string &key)
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	if (field_desc.datatype == "char")
	{
		eds_error e("eds_block::field: eds block field '");
		throw e << key << "' is type char[], not uint8_t";
	}

	const int offset = 2; // type + size sind nicht mitgerechnet
	int p = field_desc.pos + offset;

	uint8_t d0,d1,d2,d3;

	m_tcon.send_READ_FROM_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address, p);
	m_tcon.recv_READ_FROM_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
			&d0,&d1,&d2,&d3);

	return d0;
}

uint16_t eds_block::uint16_field(const string &key)
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	if (field_desc.datatype == "char")
	{
		eds_error e("eds_block::field: eds block field '");
		throw e << key << "' is type char[], not uint8_t";
	}

	const int offset = 2; // type + size sind nicht mitgerechnet
	int p = field_desc.pos + offset;
	uint8_t d0,d1,d2,d3;

	m_tcon.send_READ_FROM_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address, p);
	m_tcon.recv_READ_FROM_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
			&d0,&d1,&d2,&d3);

	return (d1 << 8) | d0;
}

string eds_block::strfield(const string &key)
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	if (field_desc.datatype != "char")
	{
		eds_error e("eds_block::strfield: eds block field '");
		throw e << key << "' is not type char[]";
	}

	uint8_t d0,d1,d2,d3;
	const int offset = 2; // type + size sind nicht mitgerechnet
	string s;

	for (size_t j = 0; j < field_desc.size; j += 4)
	{
		int p = field_desc.pos + offset + j;

		m_tcon.send_READ_FROM_BLOCK(m_bcon.src(), m_bcon.dst(),
				m_address >> 8, m_address, p);
		m_tcon.recv_READ_FROM_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
				&d0,&d1,&d2,&d3);

		if (d0 != 0) s += char(d0); else break;
		if (d1 != 0) s += char(d1); else break;
		if (d2 != 0) s += char(d2); else break;
		if (d3 != 0) s += char(d3); else break;
	}

	return s;
}

void eds_block::set_field_uint8(const string &key, uint8_t v)
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	if (field_desc.datatype == "char")
	{
		eds_error e("eds_block::set_field: eds block field '");
		throw e << key << "' is type char[], not uint8_t";
	}

	const int offset = 2; // type + size sind nicht mitgerechnet
	int p = field_desc.pos + offset;

	m_tcon.send_WRITE_TO_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address, p, v);
	m_tcon.recv_WRITE_TO_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src());

}

void eds_block::set_field_uint16(const string &key, uint16_t v)
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	if (field_desc.datatype == "char")
	{
		eds_error e("eds_block::set_field: eds block field '");
		throw e << key << "' is type char[], not uint8_t";
	}

	const int offset = 2; // type + size sind nicht mitgerechnet
	int p = field_desc.pos + offset;

	// Lo Byte
	m_tcon.send_WRITE_TO_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address, p + 0, v);
	m_tcon.recv_WRITE_TO_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src());

	// Hi Byte
	m_tcon.send_WRITE_TO_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address, p + 1, v >> 8);
	m_tcon.recv_WRITE_TO_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src());

}

void eds_block::set_field_str(const string &key, const string &v)
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	if (field_desc.datatype != "char")
	{
		eds_error e("eds_block::set_field: eds block field '");
		throw e << key << "' is not type char[]";
	}

	const int offset = 2; // type + size sind nicht mitgerechnet
	string s = v;

	// String auf die maximale Laenge kuerzen
	if ((s.size() + 1) > field_desc.size)
		s.resize(field_desc.size -1);

	// String chars schreiben
	size_t j;

	for (j = 0; j < s.size(); j++)
	{
		m_tcon.send_WRITE_TO_BLOCK(m_bcon.src(), m_bcon.dst(),
				m_address >> 8, m_address,
				field_desc.pos + offset + j, s[j]);
		m_tcon.recv_WRITE_TO_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src());

	}

	// Abschliessendes '\000' schreiben; das muss explizit geschehen,
	// denn ein s += \000; wuerde nicht funktionieren...
	m_tcon.send_WRITE_TO_BLOCK(m_bcon.src(), m_bcon.dst(),
			m_address >> 8, m_address,
			field_desc.pos + offset + j, 0);
	m_tcon.recv_WRITE_TO_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src());
}

const eds_block_fields_t &eds_block::fields() const
{
	return m_desc.fields;
}

const string &eds_block::field_datatype(const string &key) const
{
	const eds_block_field_desc_t &field_desc = 
		get_block_field_desc_by_key(m_desc, key);

	return field_desc.datatype;
}

//////////////////////////////////////////////////////////////////////////

eds_connection::eds_connection(board_connection &bcon) :
	m_bcon(bcon),
	m_tcon(m_bcon.tcon())
{
	update();
}

eds_connection::~eds_connection()
{
}

void eds_connection::update()
{
	m_blocks.clear();

	uint8_t hi,lo,type,size;
	uint16_t it = 0;

	do
	{
		m_tcon.send_GET_NEXT_BLOCK(m_bcon.src(), m_bcon.dst(), it >> 8, it);
		m_tcon.recv_GET_NEXT_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
				&hi,&lo,&type,&size);
		it = (hi << 8) | lo;

		if (it && type)
		{
			m_blocks.push_back(eds_block(*this,it));
		}
	}
	while (it != 0);
}

size_t eds_connection::space_free() const
{
	uint8_t hi,lo,type,size;
	uint16_t it = 0;
	size_t free = 0;

	do
	{
		m_tcon.send_GET_NEXT_BLOCK(m_bcon.src(), m_bcon.dst(), it >> 8, it);
		m_tcon.recv_GET_NEXT_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
				&hi,&lo,&type,&size);
		it = (hi << 8) | lo;

		if (it && !type)
			free += size;
	}
	while (it != 0);

	return free;
}

size_t eds_connection::space_used() const
{
	uint8_t hi,lo,type,size;
	uint16_t it = 0;
	size_t used = 0;

	do
	{
		m_tcon.send_GET_NEXT_BLOCK(m_bcon.src(), m_bcon.dst(), it >> 8, it);
		m_tcon.recv_GET_NEXT_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(),
				&hi,&lo,&type,&size);
		it = (hi << 8) | lo;

		if (it && type)
			used += size;
	}
	while (it != 0);

	return used;
}


eds_block &eds_connection::block_by_address(uint16_t address)
{
	for (eds_blocks::iterator i = m_blocks.begin();
			i != m_blocks.end(); i++)
	{
		if (i->address() == address)
			return *i;
	}

	eds_error e("no eds block with address = ");
	throw e << address << " found";
}

eds_block &eds_connection::create_block(const string &type_name)
{
	for (vector<eds_block_desc_t>::const_iterator i = 
			eds_block_descriptions.begin();
			i != eds_block_descriptions.end(); i++)
	{
		if (i->name == type_name)
		{
			m_tcon.send_ALLOCATE_BLOCK(m_bcon.src(), m_bcon.dst(), 
					i->type, i->size);
			uint8_t hi,lo;
			m_tcon.recv_ALLOCATE_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(), 
					&hi,&lo);

			uint16_t address = ((hi << 8) | lo);

			m_blocks.push_back(eds_block(*this,address));

			return block_by_address(address);
		}
	}

	eds_error e("no eds block type with name '");
	throw e << type_name << "' found";
}

void eds_connection::delete_block(const eds_block &block)
{
	for (eds_blocks::iterator i = m_blocks.begin();
			i != m_blocks.end(); i++)
	{
		if (i->address() == block.address())
		{
			m_tcon.send_FREE_BLOCK(m_bcon.src(), m_bcon.dst(), 
					block.address() >> 8, block.address());
			uint8_t res;
			m_tcon.recv_FREE_BLOCK_REPLAY(m_bcon.dst(), m_bcon.src(), 
					&res);

			if (res == 0)
			{
				m_blocks.erase(i);
				return;
			}

			eds_error e("error while deleting eds block at address ");
			throw e << block.address();
		}
	}

	eds_error e("no eds block with address = ");
	throw e << block.address() << " found";
}

void eds_connection::defragment()
{
	m_tcon.send_DEFRAGMENT(m_bcon.src(), m_bcon.dst());

	update();
}

void eds_connection::format()
{
	m_tcon.send_FORMAT(m_bcon.src(), m_bcon.dst());

	update();
}

