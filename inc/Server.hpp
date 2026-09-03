/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:43 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/03 01:07:09 by leoaguia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <string>

class Server
{
private:
	int _serverFd;
	int _port;
	std::string _password;

	void setupSocket();

	Server(const Server& other);
	Server&	operator=(const Server& other);
	// Assim temos a OCF completa e ainda prevenimos
	// Server a(6667, "senha");
	// Server b = a; -> Risco de close() duplo no destructor

public:
	Server(int port, const std::string& password);
	~Server();

	void run();
};

#endif
