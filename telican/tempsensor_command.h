#ifndef TEMPSENSOR_COMMAND_H
#define TEMPSENSOR_COMMAND_H

#include <command.h>
#include <libhcan++/transport_connection.h>

class tempsensor_command : public command
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;
	public:
		tempsensor_command(hcan::transport_connection &tcon, uint16_t src);
		virtual ~tempsensor_command();
		virtual bool exec(const string &line);
};

#endif
