#include "command_processor.h"
#include <assert.h>

command_processor::command_processor()
{
}

command_processor::~command_processor()
{
}

void command_processor::add(command &cmd)
{
	m_commands.push_back(&cmd);
}

void command_processor::exec(const string &line)
{
	for (vector<command*>::iterator i
			= m_commands.begin();
			i != m_commands.end(); i++)
	{
		assert(*i);
		if ((*i)->exec(line))
			return;
	}
}
