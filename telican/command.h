#ifndef COMMAND_H
#define COMMAND_H

#include <installation_data.h>

class command
{
	private:
	public:
		virtual ~command();
		virtual bool exec(const string &line) = 0;
};

#endif
