#ifndef SONSTIGE_COMMAND_H
#define SONSTIGE_COMMAND_H

#include <command.h>
#include <libhcan++/transport_connection.h>

class sonstige_command : public command
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;
	public:
		sonstige_command(hcan::transport_connection &tcon, uint16_t src);
		virtual ~sonstige_command();
		virtual bool exec(const string &line);
};

#endif
