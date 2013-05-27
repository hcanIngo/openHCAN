#ifndef ROLLADEN_COMMAND_H
#define ROLLADEN_COMMAND_H

#include <command.h>
#include <libhcan++/transport_connection.h>

class rolladen_command : public command
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;
	public:
		rolladen_command(hcan::transport_connection &tcon, uint16_t src);
		virtual ~rolladen_command();
		virtual bool exec(const string &line);
};

#endif
