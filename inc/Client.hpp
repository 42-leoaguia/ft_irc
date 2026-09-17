#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>

//TODO: Client class is not implemented yet. This class is a mock class required for the Channel code to compile

class Client
{
private:
	std::string _nickname;

public:
	Client(const std::string& nickname)
		: _nickname(nickname)
	{
	}

	const std::string& getNickname() const
	{
		return _nickname;
	}

	void sendMessage(const std::string& msg)
	{
		std::cout << "[" << _nickname << "] "
				  << msg << std::endl;
	}
};

#endif