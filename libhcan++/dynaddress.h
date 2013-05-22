#ifndef DYNADDRESS_H
#define DYNADDRESS_H

#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <libhcan++/traceable_error.h>

namespace hcan
{
	class dynaddress_error : public traceable_error 
	{
		public:
			dynaddress_error(const std::string &s);
	};

	class dynaddress
	{
		private:
			int m_address;
			int m_socket;
			struct sockaddr_in m_sin;
		public:
			dynaddress(const in_addr_t hcanaddressd_ip);
			virtual ~dynaddress();
			void allocate();
			uint16_t operator()();
	};
};

#endif
