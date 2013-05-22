#ifndef REEDKONTAKT_COMMAND_H
#define REEDKONTAKT_COMMAND_H

#include <command.h>
#include <libhcan++/transport_connection.h>

class reedkontakt_command : public command
{
	private:
		hcan::transport_connection &m_tcon;
		uint16_t m_src;
	public:
		reedkontakt_command(hcan::transport_connection &tcon, uint16_t src);
		virtual ~reedkontakt_command();
		virtual bool exec(const string &line);
};

#endif
