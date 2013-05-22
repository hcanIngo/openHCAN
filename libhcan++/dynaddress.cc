#include <dynaddress.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>

using namespace hcan;

dynaddress_error::dynaddress_error(const std::string &s) :
	traceable_error("hcanaddressd: " + s)
{
}

dynaddress::dynaddress(const in_addr_t hcanaddressd_ip) :
	m_address(-1),
	m_socket(-1)
{
	//struct sockaddr_in sin;
	memset(&m_sin, 0, sizeof(m_sin));
	m_sin.sin_family = AF_INET;

	if (hcanaddressd_ip == 0)
		m_sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	else
		m_sin.sin_addr.s_addr = hcanaddressd_ip;

	m_sin.sin_port = htons(3601);


}

dynaddress::~dynaddress()
{
	// TCP Verbindung zum hcanaddressd zumachen
	// und damit die Adresse freigeben
	if (m_socket != -1)
		close(m_socket);
}

void dynaddress::allocate()
{
	m_socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	int res = connect(m_socket,(const struct sockaddr*)&m_sin,sizeof(m_sin));
	if (res == 0)
	{
		int32_t tmp;
		size_t nread = read(m_socket,&tmp,sizeof(tmp));
		if (nread != sizeof(tmp))
			dynaddress_error("protocol error");

		m_address = tmp;
	}
	else
		throw dynaddress_error("could not connect to hcanaddressd");
}

uint16_t dynaddress::operator()()
{
	// Falls gar keine Adresse alloziert wurde, Error werfen:
	if (m_address == -1)
		throw dynaddress_error("no address allocated");
	return m_address;
}

