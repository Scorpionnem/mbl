#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "math/math.hpp"

namespace mbl::net
{

class	MulticastReceiver
{
	public:
		enum Event
		{
			RECV,
			NONE,
		};
	public:
		int connect(const char* addr, int port)
		{
			_fd = socket(AF_INET, SOCK_DGRAM, 0);
			if (_fd == -1)
				return (-1);

			int	yes = 1;
			if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) == -1)
				return (-1);

			_client_addr = {};
		    _client_addr.sin_family = AF_INET;
		    _client_addr.sin_port = htons(port);
			_client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(_fd, (struct sockaddr*)&_client_addr, sizeof(_client_addr)) == -1)
				return (-1);

			struct ip_mreq mreq;
    		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
      		if (inet_pton(AF_INET, addr, &mreq.imr_multiaddr) == -1)
				return (-1);
        	if (setsockopt(_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) == -1)
         		return (-1);

         	return (0);
		}
		void	disconnect()
		{
			close(_fd);
		}
		int	recv(void* data, u64 size, Event& event, u64& received_size)
		{
			if (_fd == -1)
			{
				errno = ENOTCONN;
				return (-1);
			}

			socklen_t	addr_len = sizeof(_client_addr);
			ssize_t	recv_size = recvfrom(_fd, data, size, MSG_DONTWAIT, (struct sockaddr*)&_client_addr, &addr_len);
			if (recv_size == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					event = Event::NONE;
					return (0);
				}

				return (-1);
			}

			received_size = recv_size;
			event = Event::RECV;
			return (0);
		}
	private:
		int					_fd = -1;
		struct sockaddr_in	_client_addr = {};
};

}
