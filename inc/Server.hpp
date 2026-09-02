#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>

class Server
{
private:
	int _serverFd;
	int _port;

	void setupSocket();

public:
	Server(int port);
	~Server();

	void run();
};

#endif