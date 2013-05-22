#include "data_query.h"
#include <libhcandata/data_file.h>
#include <libhcan++/traceable_error.h>
#include <boost/regex.hpp>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

/*
 * HCAN data query in RPN
 *
 * Ein DQ Ausdruck wird vom Query Parser in eine schnell zu verarbeitende
 * Form ueberfuehrt und kann dann von den query() Methoden verarbeitet
 * werden.
 *
 * Felder eines Frames:
 *
 * src
 * dst
 * proto
 * size
 * d0 .. d7
 *
 * Token Aufbau:
 *
 * Opcode: 1 byte
 * Parameter: 3 bytes
 *
 */

#define TOKEN_NOP            0x00000000
#define TOKEN_INT            0x01000000

#define TOKEN_SRC            0x10000000
#define TOKEN_DST            0x11000000
#define TOKEN_PROTO          0x12000000
#define TOKEN_SIZE           0x13000000
#define TOKEN_D0             0x14000000
#define TOKEN_D1             0x15000000
#define TOKEN_D2             0x16000000
#define TOKEN_D3             0x17000000
#define TOKEN_D4             0x18000000
#define TOKEN_D5             0x19000000
#define TOKEN_D6             0x20000000
#define TOKEN_D7             0x21000000

#define TOKEN_EQUAL          0x80000000
#define TOKEN_AND_OP         0x81000000
#define TOKEN_AND            0x82000000
#define TOKEN_OR_OP          0x83000000
#define TOKEN_OR             0x84000000
#define TOKEN_NOTEQUAL       0x85000000
#define TOKEN_NOT            0x86000000

using namespace std;
using namespace hcan;


data_query::data_query(const string &query)
{
	m_stack_ptr = 0;
	compile_query(query);
}

data_query::~data_query()
{
}

void data_query::compile_query(const string &query)
{
	size_t c = 0;
	uint32_t *code = m_code;

	istringstream ss(query);

	while (! ss.eof())
	{
		// Falls code Block zu klein...
		if (c >= sizeof(m_code) - sizeof(uint32_t))
			throw traceable_error("query code block too small");

		string word;
		ss >> word;

		//cout << "word = " << word << endl;

		// Ist es eine Zahl?
		if (boost::regex_match(word, boost::regex("^[0-9]+")))
		{
			uint32_t i = atol(word.c_str());
			*code = TOKEN_INT | (i & 0x00ffffff);
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "src")
		{
			*code = TOKEN_SRC;
			code++;
			c += sizeof(uint32_t);
			continue;
		}
		if (word == "dst")
		{
			*code = TOKEN_DST;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "proto")
		{
			*code = TOKEN_PROTO;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "size")
		{
			*code = TOKEN_SIZE;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "d0")
		{
			*code = TOKEN_D0;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "d1")
		{
			*code = TOKEN_D1;
			code++;
			c += sizeof(uint32_t);
			continue;
		}
		if (word == "d2")
		{
			*code = TOKEN_D2;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "d3")
		{
			*code = TOKEN_D3;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "d4")
		{
			*code = TOKEN_D4;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "d5")
		{
			*code = TOKEN_D5;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "d6")
		{
			*code = TOKEN_D6;
			code++;
			c += sizeof(uint32_t);
			continue;
		}
		if (word == "d7")
		{
			*code = TOKEN_D7;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "=")
		{
			*code = TOKEN_EQUAL;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "!=")
		{
			*code = TOKEN_NOTEQUAL;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "!")
		{
			*code = TOKEN_NOT;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "&")
		{
			*code = TOKEN_AND_OP;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "&&")
		{
			*code = TOKEN_AND;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

		if (word == "|")
		{
			*code = TOKEN_OR_OP;
			code++;
			c += sizeof(uint32_t);
			continue;
		}
		if (word == "||")
		{
			*code = TOKEN_OR;
			code++;
			c += sizeof(uint32_t);
			continue;
		}

	}

	// Falls code Block zu klein...
	if (c >= sizeof(m_code) - sizeof(uint32_t))
		throw traceable_error("query code block too small");

	*code = 0;
}

bool data_query::match(const data_file_frame_entry &f)
{
	int c = 0;
	uint32_t *code = m_code;

	while ((c < CODE_SIZE))
	{
		uint32_t opcode = *code & 0xff000000;
		uint32_t d1,d2;
		switch (opcode)
		{
			case TOKEN_NOP :
				if (m_stack_ptr != 1)
					throw traceable_error("invalid m_stack after operations");
				return pop();
			case TOKEN_INT :
				push(*code & 0x00ffffff);
				break;
			case TOKEN_SRC :
				push(f.src);
				break;
			case TOKEN_DST :
				push(f.dst);
				break;
			case TOKEN_PROTO :
				push(f.proto);
				break;
			case TOKEN_SIZE :
				push(f.size);
				break;
			case TOKEN_D0 :
				push(f.data[0]);
				break;
			case TOKEN_D1 :
				push(f.data[1]);
				break;
			case TOKEN_D2 :
				push(f.data[2]);
				break;
			case TOKEN_D3 :
				push(f.data[3]);
				break;
			case TOKEN_D4 :
				push(f.data[4]);
				break;
			case TOKEN_D5 :
				push(f.data[5]);
				break;
			case TOKEN_D6 :
				push(f.data[6]);
				break;
			case TOKEN_D7 :
				push(f.data[7]);
				break;
			case TOKEN_EQUAL :
				d1 = pop();
				d2 = pop();
				push (d1 == d2);
				break;
			case TOKEN_NOTEQUAL :
				d1 = pop();
				d2 = pop();
				push (d1 != d2);
				break;
			case TOKEN_NOT:
				d1 = pop();
				push (!d1);
				break;
			case TOKEN_AND_OP :
				d1 = pop();
				d2 = pop();
				push (d1 & d2);
				break;
			case TOKEN_AND:
				d1 = pop();
				d2 = pop();
				push (d1 && d2);
				break;
			case TOKEN_OR_OP :
				d1 = pop();
				d2 = pop();
				push (d1 | d2);
				break;
			case TOKEN_OR :
				d1 = pop();
				d2 = pop();
				push (d1 || d2);
				break;
			default :
				throw traceable_error("unknown opcode");
		}
		c += 4;
		code++;
	}

	throw traceable_error("code end reached");

}

void data_query::push(uint32_t data)
{
	m_stack[++m_stack_ptr] = data;
	if (m_stack_ptr > (STACK_SIZE -1))
		throw traceable_error("query m_stack overflow");
}

uint32_t data_query::pop()
{
	uint32_t data = m_stack[m_stack_ptr--];
	if (m_stack_ptr < 0)
		throw traceable_error("query m_stack underrun");

	return data;
}

