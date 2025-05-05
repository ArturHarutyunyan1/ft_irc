#include "../include/Server.hpp"
#include <signal.h>
Server *globalServer = NULL;

void handleSignal(int s) {
	(void)s;
	std::cout << "Shutting down server...\n";
	if (globalServer != NULL)
	{
		globalServer->cleanup();
		delete globalServer;
	}
	exit (0);
}

int main(int argc, char **argv)
{
	try {
		if (argc < 3) {
			throw std::runtime_error("Invalid arguments\nUsage - ./ircserv <port> <password>");
		} else if (stringToInt(argv[1]) < 1024 || stringToInt(argv[1]) > 65535) {
			throw std::runtime_error("Invalid port range\nUse values in range of 1024 to 65535");
		}
		signal(SIGINT, handleSignal);
		globalServer = new Server(stringToInt(argv[1]), argv[2]);
		if (!globalServer)
			throw std::runtime_error("Memory allocation failure");
		globalServer->start();
		delete globalServer;
		return (0);
	} catch (std::exception &e) {
		if (globalServer)
			delete globalServer;
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
