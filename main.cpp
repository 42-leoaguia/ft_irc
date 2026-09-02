#include "Server.hpp"

#include <iostream>
#include <cstdlib>
#include <stdexcept>

int main(int argc, char **argv)
{
	int port = std::atoi(argv[1]);

	try
	{
		Server server(port);
		server.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}