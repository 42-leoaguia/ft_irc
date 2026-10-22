#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <string>

class Server;
class Client;

class Command
{
public:
	virtual ~Command();

	virtual void execute(Server &server,
						 Client &client,
						 const std::vector<std::string> &params) = 0;
};

#endif