#ifndef JOIN_HPP
#define JOIN_HPP

#include "Command.hpp"

class Join : public Command
{
public:
	Join();
	virtual ~Join();

	virtual void execute(Server &server,
						 Client &client,
						 const std::vector<std::string> &params);
};

#endif
