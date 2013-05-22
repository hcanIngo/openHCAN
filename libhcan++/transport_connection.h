#ifndef HCAN_TRANSPORT_CONNECTION_H
#define HCAN_TRANSPORT_CONNECTION_H

#include <inttypes.h>
#include <libhcan++/traceable_error.h>
#include <libhcan++/hcan_transport.h>

/**
 * A transport connection is a connection to an hcan frame exchange
 * point, e.g. hcand. It is used to transport frames to and from the
 * exchange point.
 *
 * The opposite thing is a board connection; this is a virtual hcan
 * connection wich means that it is just a context, which defines with whom
 * to communicate.
 *
 */

namespace hcan
{

	class frame_message_description_error : public std::exception
	{
	};

	class frame;

	class transport_connection : public hcan_transport
	{
		private:
			int m_socket;
		protected:
		public:
			transport_connection (const in_addr_t hcand_ip = 0);
			virtual ~transport_connection() throw ();
			void keep_connection_alive();
			void ping(uint16_t src, uint16_t dst);
			void flood_ping(uint16_t src, uint16_t dst, const int load);
			void dump(bool numeric, bool color);
			void templog();
			void syslog();
			void send(const frame &f);
			virtual frame recv_frame (const uint16_t dst);
			/** receives a frame for the given destination. If too much
			 * time elapses, an transport_error exception is thrown.
			 */
			virtual frame recv_frame_with_timeout(int seconds);
			virtual int socket() const { return m_socket; };
	};

	void print_frame(const frame &f, bool numeric, bool color);

};

#endif

