#ifndef EDS_DESC_H
#define EDS_DESC_H

#include <inttypes.h>
#include <vector>
#include <string>

using namespace std;

struct eds_block_field_desc_t
{
	string datatype;
	string name;
	int pos;
	size_t size;

	eds_block_field_desc_t(const string &dt, const string &n, 
			int p, size_t s = 0) :
		datatype(dt),
		name(n),
		pos(p),
		size(s)
	{ }
};

typedef vector<eds_block_field_desc_t> eds_block_fields_t;

struct eds_block_desc_t
{
	uint8_t type;
	string name;
	size_t size;
	eds_block_fields_t fields;

	eds_block_desc_t(int type, const string &n, int s) :
		type(type),
		name(n),
		size(s)
	{ }
};

typedef vector <eds_block_desc_t> eds_blocks_t;

eds_blocks_t eds_get_block_descriptions();

#endif
