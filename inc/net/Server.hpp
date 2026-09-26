#pragma once

#include <cstring>
#include <vector>
#include <algorithm>
#include <map>
#include <cerrno>
#include <cstdint>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <stdbool.h>
#include <iostream>
#include <string>
#include "math/math.hpp"
#include "net/Packet.hpp"

namespace mbl::net {

/// Non-blocking, poll()-based TCP server accepting multiple clients.
/// Call update() then recv() in a loop each frame/tick; recv() walks pending
/// poll events one at a time until it returns NONE.
class	Server
{
	public:
		struct Client
		{
			int				fd = -1;
			std::vector<u8>	recv_bytes;
			u64				read_off = 0;
		};
		enum Event
		{
			RECV,
			DISCONNECT,
			CONNECTION,
			NONE,
		};

		~Server() {close();}
		/// Binds and listens on `port`. Returns 0 on success, -1 on error (errno set).
		int	open(int port, int max_connections = 16)
		{
			struct sockaddr_in	addr;

			_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (_fd == -1)
				return (-1);

			int	yes = 1;

			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = INADDR_ANY;
			if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
				return (-1);

			if (bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
				return (-1);

			if (listen(_fd, max_connections) == -1)
				return (-1);

			struct sockaddr_in bound;
			socklen_t len = sizeof(bound);
			if (getsockname(_fd, (struct sockaddr*)&bound, &len) == -1)
    			return (-1);

			char buf[INET_ADDRSTRLEN + 1] = {};
			inet_ntop(AF_INET, &bound.sin_addr, buf, sizeof(buf) - 1);
			_addr = buf;
			_port = ntohs(bound.sin_port);

			return (0);
		}
		/// Polls all sockets; must be called before draining events with recv().
		int	update()
		{
			_pollfds.clear();

			struct pollfd	listen_pfd = {};
			listen_pfd.fd = _fd;
			listen_pfd.events = POLLIN;
			_pollfds.push_back(listen_pfd);

			for (auto& c : _clients)
			{
				struct pollfd	pfd = {};
				pfd.fd = c.fd;
				pfd.events = POLLIN;
				_pollfds.push_back(pfd);
			}

			if (poll(_pollfds.data(), _pollfds.size(), 0) == -1)
				return (-1);

			_poll_index = 0;
			while (_poll_index < _pollfds.size())
			{
				struct pollfd&	pfd = _pollfds[_poll_index++];

				if (!(pfd.revents & (POLLIN | POLLHUP | POLLERR)))
					continue ;

				if (pfd.fd == _fd)
				{
					struct sockaddr_in	client_addr;
					socklen_t			len = sizeof(client_addr);
					int	client_fd = accept(_fd, (struct sockaddr*)&client_addr, &len);

					if (client_fd == -1)
						continue ;

					int	nodelay = 1;
					setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));

					_clients.push_back({.fd = client_fd});
					_connects.push_back(client_fd);
					continue ;
				}

				auto f = std::find_if(_clients.begin(), _clients.end(), [&pfd](const Client& c){return (c.fd == pfd.fd);});
				if (f == _clients.end())
					continue ;

				Client& client = *f;

				while (1)
				{
					u8	buf[4096] = {};

					ssize_t	size = ::recv(pfd.fd, buf, sizeof(buf), MSG_DONTWAIT);
					if (size <= 0)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break ;
						else if (size == -1)
						{
							perror("recv");
							break ;
						}
					}
					else if (size == 0)
					{
						_disconnects.push_back(pfd.fd);
						disconnect(pfd.fd);
						break ;
					}

					client.recv_bytes.insert(client.recv_bytes.end(), buf, buf + size);
				}
			}
			return (0);
		}
		/// Pops the next pending event (RECV/DISCONNECT/CONNECTION/NONE) from the last update().
		int	recv(void* data, u64 size, Event& event, u64& received_size, int& fd)
		{
			if (_disconnects.size())
			{
				fd = _disconnects.back();
				_disconnects.pop_back();
				return (Event::DISCONNECT);
			}
			else if (_connects.size())
			{
				fd = _connects.back();
				_connects.pop_back();
				return (Event::CONNECTION);
			}
			for (auto& c : _clients)
			{
				Packet::SizeHeader	hdr = {};
				if (_recv_peek(&hdr, sizeof(hdr), c) == -1)
				{
					event = Event::NONE;
					return (0);
				}

				if (hdr.magic != MBL_PCKT_MAGIC)
				{
					std::cerr << "Server: Invalid packet magic" << std::endl;
					return (-1);
				}
				if (size < hdr.size)
				{
					std::cerr << "Server: Buffer size too small" << std::endl;
					return (-1);
				}

				if (_recv_peek(data, hdr.size, c, sizeof(hdr)) == -1)
				{
					event = Event::NONE;
					return (0);
				}

				_recv(&hdr, sizeof(hdr), c);
				if (_recv(data, hdr.size, c) == -1)
					return (-1);

				if (_private_packet(c.fd, data, hdr.size))
				{
					event = Event::NONE;
					return (0);
				}

				fd = c.fd;
				received_size = hdr.size;
				event = Event::RECV;
				return (0);
			}

			event = Event::NONE;
			return (0);
		}
		/// Non-blocking send to one client.
		int	send(int fd, const void* data, u64 size)
		{
			Packet::SizeHeader	hdr = {.size = size};
			if (::send(fd, &hdr, sizeof(hdr), MSG_WAITALL | MSG_NOSIGNAL) == -1)
				return (-1);

			return (::send(fd, data, size, MSG_WAITALL | MSG_NOSIGNAL));
		}
		/// Non-blocking send to every connected client.
		int	send_all(const void* data, u64 size)
		{
			for (auto& c : _clients)
			{
				if (send(c.fd, data, size) == -1)
					return (-1);
			}
			return (0);
		}
		int	send_all_except(int fd, const void* data, u64 size)
		{
			for (auto& c : _clients)
			{
				if (c.fd == fd)
					continue ;

				if (send(c.fd, data, size) == -1)
					return (-1);
			}
			return (0);
		}
		void	close()
		{
			for (auto& c : _clients)
				::close(c.fd);
			_clients.clear();
			::close(_fd);
		}

		const std::string&	addr() const {return (_addr);}
		int	port() const {return (_port);}
		void	disconnect(int fd)
		{
			::close(fd);
			_clients.erase(std::remove_if(_clients.begin(), _clients.end(), [&fd](const Server::Client& client)
				{
					return (fd == client.fd);
				}), _clients.end());
		}
	private:
		int	_private_packet(int fd, void* data, u64 size)
		{
			if (size < sizeof(net::Packet::Header))
				return (0);

			net::Packet::Header*	hdr = reinterpret_cast<net::Packet::Header*>(data);

			if (hdr->magic != MBL_PCKT_MAGIC)
				return (0);

			switch (hdr->type)
			{
				case RTTREQUEST_TYPE:
				{
					net::Packet::RTTRequest*	request_pckt = reinterpret_cast<net::Packet::RTTRequest*>(data);

					mbl::net::Packet::RTTReply	repl = {};
					repl.ts = request_pckt->ts;
					send(fd, &repl, sizeof(repl));
					break ;
				}
				default:
					return (0);
			}
			return (1);
		}
		int _recv_peek(void* data, u64 size, Client& client, u64 offset = 0)
		{
		    u8* buf = reinterpret_cast<u8*>(data);
		    if (client.recv_bytes.size() - client.read_off < size + offset)
		        return (-1);
		    std::memcpy(buf, client.recv_bytes.data() + client.read_off + offset, size);
		    return (0);
		}

		int _recv(void* data, u64 size, Client& client)
		{
		    u8* buf = reinterpret_cast<u8*>(data);
		    if (client.recv_bytes.size() - client.read_off < size)
		        return (-1);
		    std::memcpy(buf, client.recv_bytes.data() + client.read_off, size);
		    client.read_off += size;

		    if (client.read_off > 65536 || client.read_off == client.recv_bytes.size())
		    {
		        client.recv_bytes.erase(client.recv_bytes.begin(), client.recv_bytes.begin() + client.read_off);
		        client.read_off = 0;
		    }
		    return (0);
		}

		int					_port = 0;
		std::string			_addr;

		int					_fd = -1;
		std::vector<Server::Client>		_clients;

		std::vector<int>				_disconnects;
		std::vector<int>				_connects;

		std::vector<struct pollfd>	_pollfds;
		size_t						_poll_index = 0;
};
}

/*
	void	server(int port)
	{
		net::Server	server;

		if (server.open(port) == -1)
			return ;

		std::cout << server.addr() << ":" << server.port() << std::endl;

		bool	running = true;
		while (running)
		{
			if (server.update() == -1)
				break ;

			net::Server::Event	event;
			u8					buf[4096];
			u64					size;
			int					fd;

			do
			{
				if (server.recv(buf, sizeof(buf), event, size, fd) == -1)
				{
					running = false;
					break ;
				}

				if (event == net::Server::Event::CONNECTION)
					std::cout << "client " << fd << " connected" << std::endl;
				else if (event == net::Server::Event::DISCONNECT)
					std::cout << "client " << fd << " disconnected" << std::endl;
				else if (event == net::Server::Event::RECV)
					server.send(fd, buf, size);
			} while (event != net::Server::Event::NONE);
		}
		server.close();
	}
 */
