#ifndef EDS_DESC_H
#define EDS_DESC_H

#include <inttypes.h>
#include <vector>
#include <string>

using namespace std;

struct eds_block_field_bit_desc_t
{
	int no;
	string description;

	eds_block_field_bit_desc_t(int n, const string &desc) :
		no(n),
		description(desc)
	{ }
};

typedef vector<eds_block_field_bit_desc_t> eds_block_field_bits_t;

struct eds_block_field_desc_t
{
	string datatype;
	string name;
	int pos;
	size_t size;
	eds_block_field_bits_t bits;

	eds_block_field_desc_t(const string &dt, const string &n, 
			int p, eds_block_field_bits_t b, size_t s = 0) :
		datatype(dt),
		name(n),
		pos(p),
		size(s),
		bits(b)
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
