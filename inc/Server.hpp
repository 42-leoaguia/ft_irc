/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:43 by liafonse          #+#    #+#             */
/*   Updated: 2026/10/27 22:29:42 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <csignal>	// std::sig_atomic_t
#include <map>		// std::map
#include <string>	// std::string
#include <vector>	// std::vector

class Client;
class Channel;
class Command;
class Message;

class Server
{
	private:
		// Uma entrada da tabela de comandos
		struct CommandHandler
		{
			Command	*cmd;				// o Server e o dono: apagado no destrutor
			bool	needsRegistration;	// false so para PASS, NICK, USER, QUIT e PING
		};

		// Regra: _pfds[0] = socket de escuta. O resto são clientes
		std::vector<struct pollfd>	_pfds;
		int							_serverFd;
		int							_port;
		std::string					_password;
		std::vector<int>			_toRemove;

		// Liga cada fd ao seu Client. O Server é o dono desses objetos
		std::map<int, Client*>		_clients;

		// Liga cada nome ao seu Channel. Também é o Server que apaga; quem cria é o JOIN (#18)
		std::map<std::string, Channel>	_channels;

		// Nome do comando (em maiusculas) -> handler. Preenchida so em initCommands()
		std::map<std::string, CommandHandler>	_commands;

		// Levantada pelo SIGINT (Ctrl+C): run() termina a rodada e sai do loop.
		// static porque o handler de sinal não recebe o objeto
		static volatile std::sig_atomic_t	_stop;

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
		void	disconnect(int fd);

		// Auxiliar: Remove um fd do vetor _pfds
		void	removePfd(int fd);
		// Auxiliar: tira o cliente de todos os canais; canal vazio é apagado
		void	removeFromChannels(Client *client);

		// Tabela de comandos: comando novo = arquivo novo + uma linha em initCommands()
		void	initCommands();
		void	addCommand(const char *name, Command *cmd, bool needsRegistration);
		void	deleteCommands();
		// Entrega uma mensagem ao handler do comando, ou responde 421/451
		void	dispatch(Client &client, const Message &msg);

	public:
		Server(int port, const std::string& password);
		~Server();

		Channel *getChannel (const std::string& name);
		std::map<std::string, Channel>& getChannels();
		Channel& addChannel(const std::string& name);
		void removeChannel(const std::string &name);

		void run();

		// Handler de SIGINT, instalado em main(): só levanta _stop
		static void	handleSigint(int signum);
};

#endif
