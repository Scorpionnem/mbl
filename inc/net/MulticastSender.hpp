#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "math/math.hpp"

namespace mbl::net
{

class	MulticastSender
{
	public:
		int	open(const char* addr, int port)
		{
			_fd = socket(AF_INET, SOCK_DGRAM, 0);
			if (_fd == -1)
				return (-1);

			_serv_addr = {};
		    _serv_addr.sin_family = AF_INET;
		    _serv_addr.sin_port = htons(port);
			if (inet_pton(AF_INET, addr, &_serv_addr.sin_addr) == -1)
				return (-1);

			return (0);
		}
		void	close()
		{
			::close(_fd);
		}
		int	send(const void* data, u64 size)
		{
			if (_fd == -1)
			{
				errno = ENOTCONN;
				return (-1);
			}
			return (sendto(_fd, data, size, 0, (struct sockaddr*)&_serv_addr, sizeof(_serv_addr)));
		}
	private:
		int					_fd = -1;
		struct sockaddr_in	_serv_addr = {};
};
}
