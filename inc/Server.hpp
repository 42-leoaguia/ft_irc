/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:43 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/02 16:31:17 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>

class Server
{
private:
	int _serverFd;
	int _port;
	std::string _password;

	void setupSocket();

public:
	Server(int port);
	~Server();

	void run();
};

#endif