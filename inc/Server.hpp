/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leoaguia <leoaguia@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:43 by liafonse          #+#    #+#             */
/*   Updated: 2026/09/13 01:25:39 by leoaguia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <string>	// std::string
#include <vector>	// std::vector

class Server
{
	private:
		// Regra: _pfds[0] = socket de escuta. O resto são clientes
		std::vector<struct pollfd>	_pfds;
		int							_serverFd;
		int							_port;
		std::string					_password;
		std::vector<int>			_toRemove;


		// Completei a OCF para evitar uma cópia do server
		Server();
		Server(const Server& other);
		Server&	operator=(const Server& other);

		// Método do socket
		void setupSocket();

		// 4 métodos chamados pelo loop()
		void	acceptClient();
		void	readFrom(int fd);
		void	writeTo(int fd);	// stub
		void	disconnect(int fd);	// stub

		// Auxiliar: Remove um fd do vetor _pfds
		void	removePfd(int fd);

	public:
		Server(int port, const std::string& password);
		~Server();

		void run();
};

#endif
