#pragma once

#include <cerrno>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include <stdbool.h>
#include <cstring>
#include <vector>
#include <cstdint>
#include "math/math.hpp"
#include "net/Packet.hpp"
#include "utils/Chrono.hpp"

namespace mbl { namespace net {

/// Non-blocking TCP client (recv() never blocks; call it in a loop and check `event`).
class	Client
{
	public:
		enum Event
		{
			RECV,
			DISCONNECT,
			NONE,
		};
	public:
		Client() {}
		~Client()
		{
			disconnect();
		}
		/// Connects to address:port. Returns 0 on success, -1 on error (errno set).
		int connect(const char* address, int port)
		{
			struct sockaddr_in	serv_addr;

			_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (_fd == -1)
				return (-1);

			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(port);
			if (inet_pton(AF_INET, address, &serv_addr.sin_addr) == -1)
				return (-1);

			if (::connect(_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
				return (-1);

			int	nodelay = 1;
			if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay)) == -1)
				return (-1);

			_addr = address;
			_port = ntohs(serv_addr.sin_port);
			return (0);
		}
		void	disconnect()
		{
			if (_fd != -1)
			{
				close(_fd);
				_fd = -1;
			}
		}
		#define CLIENT_RTT_DELAY (1.0f / 2.0f)
		int	update()
		{
			if (_disconnect == true)
				return (0);

			if (_rtt_chrono.get() > CLIENT_RTT_DELAY)
			{
				_rtt_chrono.start();
				mbl::net::Packet::RTTRequest	req = {};
				req.ts = mbl::utils::Chrono::getTimestampMS();
				send(&req, sizeof(req));
			}

			while (1)
			{
				u8	buf[4096] = {};

				ssize_t	size = ::recv(_fd, buf, sizeof(buf), MSG_DONTWAIT);
				if (size == -1)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break ;
					return (-1); // error
				}
				else if (size == 0)
				{
					_disconnect = true;
					return (0);
				}

				_recv_bytes.insert(_recv_bytes.end(), buf, buf + size);
			}
			return (0);
		}
		/// Non-blocking receive. Sets event to RECV/DISCONNECT/NONE; returns -1 on error.
		int	recv(void* data, u64 size, Event& event, u64& received_size)
		{
			if (_fd == -1)
			{
				errno = ENOTCONN;
				return (-1);
			}

			if (_disconnect)
			{
				disconnect();
				event = Event::DISCONNECT;
				return (0);
			}

			Packet::SizeHeader	hdr = {};
			if (_recv_peek(&hdr, sizeof(hdr)) == -1)
			{
				event = Event::NONE;
				return (0);
			}

			if (hdr.magic != MBL_PCKT_MAGIC)
			{
				std::cerr << "Invalid packet magic" << std::endl;
				return (-1);
			}
			if (size < hdr.size)
			{
				std::cerr << "Buffer size too small" << std::endl;
				return (-1);
			}

			if (_recv_peek(data, hdr.size, sizeof(hdr)) == -1)
			{
				event = Event::NONE;
				return (0);
			}

			_recv(&hdr, sizeof(hdr));
			if (_recv(data, hdr.size) == -1)
				return (-1);

			if (_private_packet(data, size))
			{
				event = Event::NONE;
				return (0);
			}
			received_size = hdr.size;
			event = Event::RECV;
			return (0);
		}
		/// Blocking send (waits until all `size` bytes are sent).
		int	send(const void* data, u64 size)
		{
			Packet::SizeHeader	hdr = {.size = size};
			if (::send(_fd, &hdr, sizeof(hdr), MSG_WAITALL | MSG_NOSIGNAL) == -1)
				return (-1);

			return (::send(_fd, data, size, MSG_WAITALL | MSG_NOSIGNAL));
		}
		u64	rtt() {return (_rtt);}
		std::string	addr() {return (_addr);}
		int	port() {return (_port);}
	private:
		int _recv_peek(void* data, u64 size, u64 offset = 0)
		{
		    u8* buf = reinterpret_cast<u8*>(data);
		    if (_recv_bytes.size() - _read_off < size + offset)
		        return (-1);
		    std::memcpy(buf, _recv_bytes.data() + _read_off + offset, size);
		    return (0);
		}

		int _recv(void* data, u64 size)
		{
		    u8* buf = reinterpret_cast<u8*>(data);
		    if (_recv_bytes.size() - _read_off < size)
		        return (-1);
		    std::memcpy(buf, _recv_bytes.data() + _read_off, size);
		    _read_off += size;

		    if (_read_off > 65536 || _read_off == _recv_bytes.size())
		    {
		        _recv_bytes.erase(_recv_bytes.begin(), _recv_bytes.begin() + _read_off);
		        _read_off = 0;
		    }
		    return (0);
		}
		int	_private_packet(void* data, u64 size)
		{
			if (size < sizeof(net::Packet::Header))
				return (0);

			net::Packet::Header*	hdr = reinterpret_cast<net::Packet::Header*>(data);

			if (hdr->magic != MBL_PCKT_MAGIC)
				return (0);

			switch (hdr->type)
			{
				case RTTREPLY_TYPE: // rtt reply
				{
					net::Packet::RTTReply*	reply_pckt = reinterpret_cast<net::Packet::RTTReply*>(data);

					_rtt = utils::Chrono::getTimestampMS() - reply_pckt->ts;
					break ;
				}
				case RTTREQUEST_TYPE:
				{
					net::Packet::RTTRequest*	request_pckt = reinterpret_cast<net::Packet::RTTRequest*>(data);
					mbl::net::Packet::RTTReply	req;

					req.ts = request_pckt->ts;
					send(&req, sizeof(req));
				}
				default:
					return (0);
			}
			return (1);
		}

		std::string	_addr;
		int			_port;

		utils::Chrono	_rtt_chrono;
		u64	_rtt = 0;
		std::vector<u8>	_recv_bytes;
		u64             _read_off = 0;

		int	_fd = -1;
		bool	_disconnect = false;
};
}}
/*
	void	client(const char* addr, int port)
	{
		net::Client	client;

		if (client.connect(addr, port) == -1)
			return ;

		bool	running = true;
		while (running)
		{
			net::Client::Event	event;
			u8					buf[4096];
			u64					size;

			do
			{
				if (client.recv(buf, sizeof(buf), event, size) == -1)
				{
					running = false;
					break ;
				}
				if (event == net::Client::Event::DISCONNECT)
				{
					running = false;
					break ;
				}

				std::cout << '\r' << "waiting..." << std::flush;
			} while (event != net::Client::Event::NONE);
		}
		client.disconnect();
	}
 */
