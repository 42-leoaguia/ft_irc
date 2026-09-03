/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:52 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/03 00:49:40 by leoaguia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// -Iinc/ on Makefile, so no need to "../../inc/Server.hpp"
#include "Server.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <fcntl.h>

Server::Server(int port, const std::string& password)
	: _serverFd(-1), _port(port), _password(password)
{
	setupSocket();
}

Server::~Server()
{
	if (_serverFd != -1)
		close(_serverFd);
}

void Server::setupSocket()
{
	// Create the server socket and make the listening socket non-blocking
	_serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (_serverFd == -1)
		throw std::runtime_error("socket() failed");

	// Allow the port to be reused immediately after closing the server
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR,
		&opt, sizeof(opt)) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("setsockopt() failed");
	}

	// Make the listening socket non-blocking
	// if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
	// {
	// 	close(_serverFd);
	// 	_serverFd = -1;
	// 	throw std::runtime_error("fcntl() failed");
	// }

	// Set the address
	sockaddr_in address;
	std::memset(&address, 0, sizeof(address));

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(_port);

	// Bind the socket to the port
	if (bind(_serverFd, reinterpret_cast<sockaddr *>(&address),
		sizeof(address)) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("bind() failed");
	}

	// Start listening for incoming connections
	if (listen(_serverFd, SOMAXCONN) == -1)
	{
		close(_serverFd);
		_serverFd = -1;
		throw std::runtime_error("listen() failed");
	}

	std::cout << "Server listening on port "
			  << _port << std::endl;
}

void Server::run()
{
	struct pollfd pfd;

	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	pfd.revents = 0;

	while (true)
	{
		int result = poll(&pfd, 1, -1);

		if (result == -1)
			throw std::runtime_error("poll() failed");

		if (pfd.revents & POLLIN)
		{
			sockaddr_in clientAddress;
			socklen_t clientSize = sizeof(clientAddress);

			int clientFd = accept(
				_serverFd,
				reinterpret_cast<sockaddr *>(&clientAddress),
				&clientSize
			);

			if (clientFd == -1)
				throw std::runtime_error("accept() failed");

			std::cout << "Client connected: "
					  << clientFd << std::endl;

			// Temporary for this issue.
			// Later, the client FD will be stored and added to poll().
			close(clientFd);
		}
	}
}
