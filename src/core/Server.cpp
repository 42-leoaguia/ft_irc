/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 16:19:52 by liafonse          #+#    #+#             */
/*   Updated: 2026/11/26 15:48:25 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Replies.hpp"
#include "Command.hpp"
#include "Join.hpp"
#include "Topic.hpp"
#include "Part.hpp"
#include "Mode.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <new>			// std::bad_alloc
#include <utility>		// std::make_pair

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// inet_ntoa

#include <unistd.h>
#include <fcntl.h>

volatile std::sig_atomic_t	Server::_stop = 0;

/* Constructor */
Server::Server(int port, const std::string& password)
	: _serverFd(-1), _port(port), _password(password)
{
	// Se o construtor lanca, o destrutor nao roda: desfaz aqui o que ja foi feito
	try
	{
		initCommands();
		setupSocket();
	}
	catch (...)
	{
		deleteCommands();
		if (_serverFd != -1)
			close(_serverFd);
		throw ;
	}
}

/* Destructor: apaga os canais, fecha e apaga os clientes que ainda estiverem
conectados. Roda no Ctrl+C: run() sai do loop e o Server sai de escopo em main() */
Server::~Server()
{
	// Canais primeiro: guardam Client*, mas nunca apagam clientes
	_channels.clear();
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	_clients.clear();
	deleteCommands();
	if (_serverFd != -1)
		close(_serverFd);
}

/*
initCommands(): O UNICO lugar que conhece nomes de comando. Comando novo =
arquivo novo + uma linha aqui (e o #include do header dele no topo).
O terceiro argumento diz se o comando exige registro completo: so PASS, NICK,
USER, QUIT e PING passam com false, o resto recebe 451 antes do registro.
*/
void	Server::initCommands()
{
	addCommand("JOIN", new Join(), true);
	addCommand("PART", new Part(), true);
	addCommand("TOPIC", new Topic(), true);
	addCommand("MODE", new Mode(), true);
}

/*
addCommand(): So chamado por initCommands(). Recebe o nome como const char*
para nada alocar entre o new do handler e esta chamada: se a insercao falha,
o handler e apagado aqui e nada vaza.
*/
void	Server::addCommand(const char *name, Command *cmd, bool needsRegistration)
{
	CommandHandler	handler;

	handler.cmd = cmd;
	handler.needsRegistration = needsRegistration;
	try
	{
		// insert() nao substitui: nome repetido e erro de quem registrou
		if (!_commands.insert(std::make_pair(std::string(name), handler)).second)
			throw std::logic_error(std::string("command registered twice: ") + name);
	}
	catch (...)
	{
		delete cmd;
		throw ;
	}
}

void	Server::deleteCommands()
{
	std::map<std::string, CommandHandler>::iterator	it;

	for (it = _commands.begin(); it != _commands.end(); ++it)
		delete it->second.cmd;
	_commands.clear();
}

void Server::setupSocket()
{
	// 1. Create the server socket
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
		throw std::runtime_error("socket() failed");

	// 2. Allow the port to be reused immediately after closing the server
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
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
Ctrl+C (SIGINT) ends the loop; ~Server() then closes and deletes everything.
*/
void Server::run()
{
	// Um SIGINT que chegue entre este teste e a entrada no poll() so e visto
	// no proximo evento (ou no proximo Ctrl+C)
	while (!_stop)
	{
		int	ready = poll(&_pfds[0], _pfds.size(), -1);

		// -1: um sinal interrompeu o poll() (o SIGINT faz isso) ou ele falhou.
		// Sem olhar errno: volta ao while, que sai se foi o SIGINT
		if (ready == -1)
			continue;

		// erase() encurta o vetor fazendo ++i pular um fd
		_toRemove.clear();

		// Tamanho capturado ANTES do laço
		size_t	count = _pfds.size();

		for (size_t i = 0; i < count; ++i)
		{
			short	revents = _pfds[i].revents;
			int		fd = _pfds[i].fd;

			if (revents == 0)
				continue;

			if ((revents & POLLHUP) || (revents & POLLERR))
			{
				_toRemove.push_back(fd);
				continue;
			}

			if (i == 0)
			{
				if (revents & POLLIN)
					acceptClient();
				continue;
			}

			if (revents & POLLIN)
				readFrom(fd);
			if (revents & POLLOUT)
				writeTo(fd);
		}

		// Laco terminado, seguro encurtar vetor
		for (size_t i = 0; i < _toRemove.size(); ++i)
			disconnect(_toRemove[i]);
	}
	std::cout << "Server shutting down" << std::endl;
}

/*
handleSigint(): Handler de SIGINT. Num handler so e seguro escrever numa
volatile sig_atomic_t: nada de cout, close ou delete aqui. Quem fecha e apaga
tudo e o destrutor, quando main() termina.
*/
void	Server::handleSigint(int signum)
{
	(void)signum;
	_stop = 1;
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
	Client			*client = NULL;

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

	// O Server e o dono do Client: criado aqui, apagado em disconnect().
	// Sem memoria recusamos so este cliente, desfazendo o que ja foi feito
	try
	{
		std::string	host(inet_ntoa(clientAddress.sin_addr));

		client = new Client(clientFd, host);
		_clients[clientFd] = client;
		_pfds.push_back(pfd);
	}
	catch (const std::bad_alloc&)
	{
		_clients.erase(clientFd);
		delete client;
		close(clientFd);
		return ;
	}

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

/*
replyTarget(): Quem recebe uma resposta numerica: o nick, ou "*" enquanto o
cliente ainda nao tem um (RFC 2812)
*/
static std::string	replyTarget(const Client &client)
{
	if (client.getNickname().empty())
		return ("*");
	return (client.getNickname());
}

/*
dispatch(): Procura o comando na tabela e chama o handler dele. O Server nao
conhece nome de comando nenhum aqui: tudo vem de initCommands().
  comando que nao esta na tabela                 -> 421 ERR_UNKNOWNCOMMAND
  exige registro e o cliente nao esta registrado -> 451 ERR_NOTREGISTERED
*/
void	Server::dispatch(Client &client, const Message &msg)
{
	std::map<std::string, CommandHandler>::const_iterator	it;

	it = _commands.find(msg.getCommand());
	if (it == _commands.end())
	{
		client.sendMessage(std::string(":ircserv ") + ERR_UNKNOWNCOMMAND + " "
			+ replyTarget(client) + " " + msg.getCommand() + " :"
			+ ERR_UNKNOWNCOMMAND_TEXT);
		return ;
	}
	if (it->second.needsRegistration && !client.isRegistered())
	{
		client.sendMessage(std::string(":ircserv ") + ERR_NOTREGISTERED + " "
			+ replyTarget(client) + " :" + ERR_NOTREGISTERED_TEXT);
		return ;
	}
	it->second.cmd->execute(*this, client, msg.getParams());
}

/*
readFrom(): Reads whatever arrived on a client fd.
TCP entrega bytes, nao comandos: um recv() pode trazer meio comando, um
comando inteiro ou varios. Os bytes vao para o buffer do cliente e so saem
de la como linhas completas (Client::extractLine).
*/
void	Server::readFrom(int fd)
{
	char								buffer[512];
	ssize_t								bytes;
	std::string							line;
	std::map<int, Client*>::iterator	it;
	Client								*client;

	// Todo fd de cliente em _pfds tem um Client (acceptClient/disconnect)
	it = _clients.find(fd);
	if (it == _clients.end())
		return ;
	client = it->second;

	bytes = recv(fd, buffer, sizeof(buffer), 0);

	// 0 = o outro lado fechou a conexao
	if (bytes == 0)
	{
		_toRemove.push_back(fd);
		return ;
	}

	// -1 = nada disponivel agora: espera o proximo poll(), sem olhar errno
	if (bytes == -1)
	{
		return ;
	}

	std::cout << "Received " << bytes << " bytes from fd " << fd << std::endl;

	// Guarda tudo o que chegou, mesmo que seja so um pedaco de comando
	client->appendToInBuffer(buffer, bytes);

	// Um recv() pode trazer varios comandos: processa todas as linhas completas.
	// O que sobrar (comando pela metade) fica no buffer esperando o proximo recv()
	while (client->extractLine(line))
	{
		Message	msg(line);

		// Linha vazia ou so com espacos nao tem comando: descartada sem resposta
		if (msg.isEmpty())
			continue ;
		std::cout << "Line from fd " << fd << ": [" << line << "]" << std::endl;
		// Nenhum comando apaga o Client aqui dentro: a remocao vai para _toRemove.
		// Um bug num comando nao pode terminar o processo (nota 0): o erro fica
		// so com este cliente. O comando pode ter parado no meio e deixado o
		// estado dele pela metade, entao ele cai no fim da rodada e o resto do
		// que mandou e descartado. Os outros clientes seguem normalmente
		try
		{
			dispatch(*client, msg);
		}
		catch (const std::exception &e)
		{
			std::cerr << "Error: command " << msg.getCommand() << " from fd "
				<< fd << " failed: " << e.what() << std::endl;
			_toRemove.push_back(fd);
			return ;
		}
	}
	// Mais de 510 bytes sem "\n" nunca vira uma linha valida (RFC 2812):
	// derruba o cliente para o buffer nao crescer sem limite
	if (client->inputOverflow())
		_toRemove.push_back(fd);
}

void	Server::writeTo(int fd)
{
	// TODO issue #6: send do buffer de saida, limpar POLLOUT
	// (void) para evitar warning
	(void)fd;
}

/*
disconnect(): Tira o cliente de tudo o que o Server guarda e so entao o apaga.
So e chamado no fim da rodada do run(): no meio do laco, encurtar _pfds faria
o indice pular um fd.
TODO issue #16: antes de sair dos canais, mandar QUIT aos outros membros
(precisa do queue() da issue #6).
*/
void	Server::disconnect(int fd)
{
	std::map<int, Client*>::iterator	it;
	Client								*client;

	// O mesmo fd pode ter sido agendado duas vezes na mesma rodada
	it = _clients.find(fd);
	if (it == _clients.end())
		return ;
	client = it->second;
	// Os canais guardam Client*: sem isto ficaria um ponteiro solto
	removeFromChannels(client);
	close(fd);
	removePfd(fd);
	_clients.erase(it);
	delete client;
	std::cout << "Client disconnected: " << fd << std::endl;
}

Channel *Server::getChannel(const std::string& name)
{
	if (_channels.find(name) != _channels.end())
		return &_channels[name];
	return NULL;
}

std::map<std::string, Channel>& Server::getChannels()
{
	return _channels;
}

Channel& Server::addChannel(const std::string& name)
{
	if (!this->getChannel(name))
		_channels[name] = Channel(name);
	return _channels[name];
}

void Server::removeChannel(const std::string &name)
{
	_channels.erase(name);
}

/*
removeFromChannels(): Tira o cliente de todos os canais, como membro e como
operador. Um canal que fica sem ninguem deixa de existir, como no IRC.
*/
void	Server::removeFromChannels(Client *client)
{
	std::map<std::string, Channel>::iterator	it = _channels.begin();

	while (it != _channels.end())
	{
		it->second.removeMember(*client);
		if (it->second.getMembers().empty())
			// erase() nao devolve o seguinte, entao it++ avanca antes
			_channels.erase(it++);
		else
			++it;
	}
}
