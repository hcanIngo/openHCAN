#ifndef LAMPE_COMMAND_H
#define LAMPE_COMMAND_H

#include <command.h>
#include <libhcan++/transport_connection.h>

class lampe_command : public command
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;
	public:
		lampe_command(hcan::transport_connection &tcon, uint16_t src);
		virtual ~lampe_command();
		virtual bool exec(const string &line);
};

#endif
