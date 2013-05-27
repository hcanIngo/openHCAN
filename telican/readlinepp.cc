#include <readlinepp.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <readline/readline.h>
#include <readline/history.h>

using namespace std;
using namespace readlinepp;

string readlinepp::read_line(const string &prompt)
{
	char *s = ::readline(prompt.c_str());

	if (! s)
	{
		throw end_of_input();
	}

	string s2(s);
	free(s);

	return s2;
}

history::history(const string &filename) :
	m_histfile(filename)
{
	using_history();
	read_history(m_histfile.c_str());
}

history::~history()
{
	write_history(m_histfile.c_str());
}

void history::add (const string &line)
{
	add_history(line.c_str());
}

