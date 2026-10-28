#ifndef PART_HPP
#define PART_HPP

#include "Command.hpp"

class Part : public Command
{
public:
	Part();
	virtual ~Part();

	virtual void execute(Server &server,
						 Client &client,
						 const std::vector<std::string> &params);
};

#endif