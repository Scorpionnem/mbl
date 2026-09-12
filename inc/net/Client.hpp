#pragma once

#include <cerrno>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <errno.h>
#include <stdbool.h>
#include <cstdint>
#include "math/math.hpp"
#include "net/Packet.hpp"

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
		#define CLIENT_RTT_DELAY (1.0f / 3.0f) // 3 rtt per second
		void	update()
		{
			if (_rtt_chrono.get() > CLIENT_RTT_DELAY)
			{
				_rtt_chrono.start();
				_rtt_send = utils::Chrono::getTimestampMS();
				mbl::net::Packet::RTTRequest	req;
				send(&req, sizeof(req));
			}
		}
		/// Non-blocking receive. Sets event to RECV/DISCONNECT/NONE; returns -1 on error.
		int	recv(void* data, u64 size, Event& event, u64& received_size)
		{
			if (_fd == -1)
			{
				errno = ENOTCONN;
				return (-1);
			}

			ssize_t	recv_size = ::recv(_fd, data, size, MSG_DONTWAIT);
			if (recv_size == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					event = Event::NONE;
					return (0);
				}

				return (-1);
			}
			if (recv_size == 0)
			{
				disconnect();
				event = Event::DISCONNECT;
				return (0);
			}

			if (_private_packet(data, size))
			{
				event = Event::NONE;
				return (0);
			}
			received_size = recv_size;
			event = Event::RECV;
			return (0);
		}
		/// Blocking send (waits until all `size` bytes are sent).
		int	send(const void* data, u64 size)
		{
			return (::send(_fd, data, size, MSG_WAITALL));
		}
		u64	rtt() {return (_rtt);}
	private:
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
					_rtt = utils::Chrono::getTimestampMS() - _rtt_send;
					break ;
				}
				case RTTREQUEST_TYPE:
				{
					mbl::net::Packet::RTTReply	req;
					send(&req, sizeof(req));
				}
				default:
					return (0);
			}
			return (1);
		}

		utils::Chrono	_rtt_chrono;
		u64	_rtt_send = 0;
		u64	_rtt = 0;

		int	_fd = -1;
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
