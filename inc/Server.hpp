/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:43 by liafonse          #+#    #+#             */
/*   Updated: 2026/10/13 18:03:46 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <map>		// std::map
#include <string>	// std::string
#include <vector>	// std::vector

class Client;
class Channel;

class Server
{
	private:
		// Regra: _pfds[0] = socket de escuta. O resto são clientes
		std::vector<struct pollfd>	_pfds;
		int							_serverFd;
		int							_port;
		std::string					_password;
		std::vector<int>			_toRemove;

		// Liga cada fd ao seu Client. O Server é o dono desses objetos
		std::map<int, Client*>		_clients;

		std::map<std::string, Channel> _channels;

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

		// TODO issue 18-20 - função auxiliar de teste REMOVER NO FIM!
		void parseCommand(Client& client, const std::string& line);

	public:
		Server(int port, const std::string& password);
		~Server();

		Channel *getChannel (const std::string& name);
		std::map<std::string, Channel>& getChannels();
		Channel& addChannel(const std::string& name);

		void run();
};

#endif
