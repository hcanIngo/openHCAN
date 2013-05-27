#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <iostream>
#include "readline_completer.h"

using namespace readlinepp;
using namespace std;

vector<completion_generator*> completer::m_generators = 
	vector<completion_generator*>();
	
completion_generator* completer::m_current_generator = 0;

completer::completer()
{
	m_current_generator = 0;
	rl_attempted_completion_function = &completer::completion;
}

completer::~completer()
{
	rl_attempted_completion_function = 0;
}

void completer::add(completion_generator &gen)
{
	m_generators.push_back(&gen);
}

void * xmalloc (int size)
{
    void *buf;

    buf = malloc (size);
    if (!buf) {
        fprintf (stderr, "Error: Out of memory. Exiting.'n");
        exit (1);
    }

    return buf;
}

char * dupstr (const char* s) {
    char *r;

    r = (char*) xmalloc ((strlen (s) + 1));
    strcpy (r, s);
    return (r);
}


char* completer::completion_generator_func(const char* text, int state)
{
    static size_t list_index, len;

	if (!state) {
        list_index = 0;
        len = strlen (text);
    }

	//cout << " completer::completion_generator_func: " << string(text) << endl;

	if (m_current_generator)
	{
		while (list_index < m_current_generator->completions_count(
					rl_line_buffer))
		{
			string s = m_current_generator->get_completion(text, 
					list_index++);

			if (strncmp(s.c_str(), text, len) == 0)
				return (dupstr(s.c_str()));
		}
	}

	return 0;
}


char** completer::completion(const char * text, 
		int start,  int end)
{
	char **matches;
	matches = (char **)0;

	m_current_generator = 0;

	for (vector<completion_generator*>::iterator i 
			= m_generators.begin();
			i != m_generators.end(); i++)
	{
		//cout << "rl_line_buffer: " << rl_line_buffer << "\n";
		if ((*i) && ((*i)->completions_count(rl_line_buffer) > 0))
		{
			m_current_generator = (*i);
			//cout << "generator\n";
			break;
		}
	}

	if (m_current_generator)
	{
		matches = rl_completion_matches((char*) text,
				&completer::completion_generator_func);

		return (matches);
	}

	m_current_generator = 0;
	return 0;
}
