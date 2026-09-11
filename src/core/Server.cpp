/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:52 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/11 23:54:04 by leoaguia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <fcntl.h>

/* Constructor */
Server::Server(int port, const std::string& password)
	: _serverFd(-1), _port(port), _password(password)
{
	setupSocket();
}

/* Destructor */
Server::~Server()
{
	if (_serverFd != -1)
		close(_serverFd);
}

void Server::setupSocket()
{
	// 1. Create the server socket
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
		throw std::runtime_error("socket() failed");

	// 2. Allow the port to be reused immediately after closing the server
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt)) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("setsockopt() failed");
	}

	// 3. Make the listening socket non-blocking
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("fcntl() failed");
	}

	// 4. Set the address
	sockaddr_in	address;
	std::memset(&address, 0, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(_port);

	// 5. Bind the socket to the port
	if (bind(_serverFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("bind() failed");
	}

	// 6. Start listening for incoming connections
	if (listen(_serverFd, SOMAXCONN) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("listen() failed");
	}

	// 7. Register _pfds[0] as the listening socket
	struct pollfd	pfd;

	pfd.fd = _serverFd;
	pfd.events = POLLIN;	// Awares us when there is anything to read
	pfd.revents = 0;
	_pfds.push_back(pfd);

	std::cout << "Server listening on port " << _port << std::endl;
}

/*
Events Loop: Server core
1 turn
- Ask if anyone has news
- Deal with any news
- Back to sleep mode
*/
void Server::run()
{
	while (true)
	{
		// &_pfds[0] funciona porque std::vector garante memória contigua.
		// Dúvida: O que é memória contigua?
		// Entendi que _pfds é um vetor C++ de pollfd
		// Mas o que é uma API de C?
		int	ready = poll(&_pfds[0], _pfds.size(), -1);

		// TODO issue #7: O tratamento correto do -1 depende do handler de SIGINT
		// Por enquanto voltamos ao início
		if (ready == -1)
			continue;

		// erase() encurta o vetor fazendo i++ pular um fd
		std::vector<int>	toRemove;

		size_t	count = _pfds.size();

		for (size_t i = 0; i < count; ++i)
		{
			short	revents = _pfds[i].revents;
			int		fd = _pfds[i].fd;

			if (revents == 0)
				continue;

			if ((revents & POLLHUP) || (revents & POLLERR))
			{
				toRemove.push_back(fd);
				continue;
			}

			if (i == 0)
			{
				if (revents && POLLIN)
					acceptClient();
				continue;
			}

			if (revents & POLLIN)
				readFrom(fd);
			if (revents & POLLOUT)
				writeTo(fd);
		}

		// Laco terminado, seguro encurtar vetor
		for (size_t i = 0; i < toRemove.size(); ++i)
			disconnect(toRemove[i]);
	}
}

/*
acceptClient(): Removes the first connection from the queue and watchs it.
accept(): Creates a new socket with an exclusive client fd.
*/
void	Server::acceptClient()
{
	sockaddr_in		clientAddress;
	socklen_t		clientSize = sizeof(clientAddress);
	int				clientFd;
	struct pollfd	pfd;

	clientFd = accept(_serverFd, reinterpret_cast<sockaddr *>(&clientAddress), &clientSize);

	// Um accept falho nao deve derrubar o server.
	// Um cliente que desiste da comunicacao e ignorado
	if (clientFd == -1)
		return ;

	// O fd devolvido por accept nao herda o modo nao bloqueante do socket de escuta
	// Logo, cada cliente precisa do seu proprio fcntl
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(clientFd);
		return ;
	}

	pfd.fd = clientFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_pfds.push_back(pfd);

	std::cout << "Client connected: " << clientFd << std::endl;
}

/*
removePfd(): Removes a fd from the vector that poll() is watching
*/
void	Server::removePfd(int fd)
{
	for (size_t i = 0; i < _pfds.size(); ++i)
	{
		if (_pfds[i].fd == fd)
		{
			_pfds.erase(_pfds.begin() + i);
			return ;
		}
	}
}

void	Server::readFrom(int fd)
{
	// TODO issue #5: recv -> appendToInBuffer -> while(extractLine)
	// (void) para evitar warning
	(void)fd;
}

void	Server::writeTo(int fd)
{
	// TODO issue #6: send do buffer de saida, limpar POLLOUT
	// (void) para evitar warning
	(void)fd;
}

/*
TODO issue #7: avisar os canais, apagar o objeto Client, mensagem de QUIT.
*/
void	Server::disconnect(int fd)
{
	removePfd(fd);
	close(fd);
	std::cout << "Client disconnected: " << fd << std::endl;
}
