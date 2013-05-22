#ifndef HCAN_BOARD_CONNECTION_H
#define HCAN_BOARD_CONNECTION_H

#include <inttypes.h>
#include <libhcan++/traceable_error.h>
#include <board_driver.h>

/**
 *
 * Eine transport_connection ist eine Verbindung zum hcand; sie wird
 * verwendet, um HCAN Frames mit anderen Teilnehmern auszutauschen.
 *
 * Eine board_connection ist eine virtuelle Verbindung zu einem Board
 * (Platine). Sie verwendet eine transport_connection, um die Frames
 * zum Board zu senden und zu empfangen.
 *
 */

namespace hcan
{

	class board_error : public traceable_error
	{
		public:
			board_error (const string &s);
	};

	class transport_connection;

	class board_connection
	{
		private:
			/** a reference to our transport connection */
			transport_connection &m_tcon;
			/** the board driver instance */
			board_driver_p m_driver;
			/** Our own address wich we got assigned */
			uint16_t m_src;
			/** The address of the board we are connected */
			uint16_t m_dst;
		public:
			board_connection (transport_connection &tcon, 
					uint16_t src, uint16_t dst, bool ignore_type, 
					uint8_t arch);
			virtual ~board_connection() throw ();
			transport_connection &get_transport_connection () { return m_tcon; }
			uint16_t src () const { return m_src; }
			uint16_t dst () const { return m_dst; }
			board_driver_p &get_driver () { return m_driver; }
			transport_connection &tcon() { return m_tcon; }
	};

};

#endif

