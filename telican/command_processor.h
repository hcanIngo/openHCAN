#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <command.h>
#include <vector>

class command_processor
{
	private:
		vector<command*> m_commands;
	public:
		command_processor();
		virtual ~command_processor();
		void add(command &cmd);
		void exec(const string &line);
};

#endif
