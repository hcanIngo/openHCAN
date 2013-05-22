#include <iostream>
#include <sstream>
#include <stdlib.h>

#ifndef __WIN32__
#include <execinfo.h>
#endif

#include "traceable_error.h"

using namespace std;

traceable_error::traceable_error(const string &s) :
	m_message(s)	
{
	// einen Stacktrace aufzeichnen, da dieser nach dem Wurf der
	// Exception nicht mehr so aufgebaut ist:
	//
	
	//m_trace = create_stack_trace();
}

traceable_error::~traceable_error() throw ()
{
}

vector<string> traceable_error::create_stack_trace ()
{
	vector<string> v;

#ifndef __WIN32__

	void *b[10000];
	int size = backtrace(b, 10000);
	char **s = backtrace_symbols(b, size);
	for(int i = 0; i < size; i++)
	{
		v.push_back(string(s[i]));
	}
	free(s);

#endif

	return v;
}

const vector<string> traceable_error::trace () const
{
	return m_trace;
}

const char* traceable_error::what() const throw ()
{
	return m_message.c_str();
}

traceable_error &operator << (traceable_error &e, const string &s)
{
	e.m_message += s;
	return e;
}

traceable_error &operator << (traceable_error &e, const int i)
{
	stringstream ss;
	ss << i;
	e.m_message += ss.str();
	return e;
}



std::ostream &operator << (std::ostream &o, const traceable_error &e)
{
	o << e.m_message;
	return o;
}


