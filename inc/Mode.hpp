#ifndef MODE_HPP
#define MODE_HPP

#include "Command.hpp"

class Mode : public Command
{
public:
	Mode();
	virtual ~Mode();

	virtual void execute(Server &server,
						 Client &client,
						 const std::vector<std::string> &params);
};

#endif