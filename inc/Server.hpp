/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:43 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/03 01:11:17 by leoaguia         ###   ########.fr       */
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

	// Um Server só existe com porta e senha
	// Copiar um significaria dois donos do mesmo fd. close() duplo no destrutor.
	Server();
	Server(const Server& other);
	Server&	operator=(const Server& other);

public:
	Server(int port, const std::string& password);
	~Server();

	void run();
};

#endif
