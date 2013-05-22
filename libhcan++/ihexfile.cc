#include <ihexfile.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;

ihexfile_error::ihexfile_error(const string &s) :
	traceable_error(s)
{
}

uint8_t str2hex(const string &s)
{
	uint16_t value;
	stringstream sin(s);
	sin >> setbase(16) >> value;
	return value;
}

ihexfile::ihexfile()
{
}

void ihexfile::load(const string &filename, uint16_t page_size)
{
	// Vorher aufraeumen:
	m_data.clear();

	ifstream in(filename.c_str());

	if (! in)
		throw ihexfile_error("could not open file " + filename);

	m_filename = filename;

	char buf[512];
	while (in.getline(buf,sizeof(buf)))
	{
		string line(buf);
		if (line != "")
		{
			uint16_t length = str2hex(line.substr(1,2));
			/*
			uint16_t addr = (str2hex(line.substr(3,2)) << 8) |
				str2hex(line.substr(5,2).c_str());
			*/
			uint8_t rectype = str2hex(line.substr(7,2));

			if (rectype == 0) // data record
			{
				string dataline = line.substr(9, length * 2);
				for (int i = 0; i < length; i++)
					m_data.push_back(str2hex(dataline.substr(i * 2, 2)));
			}
		}
	}

	// Align the data to m_page_size bytes
	size_t size = m_data.size();
	const size_t padding = size % page_size;

	for (uint8_t i = 0; i < (page_size - padding); i++)
		m_data.push_back(0xff);

	size = m_data.size(); // size inklusive padding bytes!
	assert(size % page_size == 0);
}

void ihexfile::copy_original_to(const string &filename) const
{
	if (m_filename != "")
	{
		stringstream ss;
		ss << "/bin/cp " << m_filename << " " << filename;
		int res = system(ss.str().c_str());
		switch (res)
		{
			case -1 : throw ihexfile_error("/bin/cp exec error");
			case 0 : break;
			default : 
			{
				stringstream ss;
				ss << "/bin/cp error: " << res;
				throw ihexfile_error(ss.str());
			}

		}
	}
	else
		throw ihexfile_error("copy_original_to: no hexfile loaded");
}

ihexfile::~ihexfile()
{
}

vector<uint8_t> ihexfile::data() const
{
	return m_data;
}

const uint16_t ihexfile::size() const
{
	return m_data.size();
}

uint16_t ihexfile::_crc16_update(uint16_t crc, const uint8_t a) const
{
	int i;

	crc ^= a;
	for (i = 0; i < 8; ++i)
	{
		if (crc & 1)
			crc = (crc >> 1) ^ 0xA001;
		else
			crc = (crc >> 1);
	}

	return crc;

}

const uint16_t ihexfile::crc16() const
{
	uint16_t crc = 0xffff;

	for (size_t i = 0; i < m_data.size()>>1; i++)
	{
		// Die beiden Bytes sind wegen LE "verdreht"
		const uint8_t a = m_data[i*2];
		const uint8_t b = m_data[i*2+1];
		crc = ihexfile::_crc16_update(crc, b);
		crc = ihexfile::_crc16_update(crc, a);
	}

	return crc;
}

