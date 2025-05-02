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
	if (argc < 3) {
		std::cerr << "Invalid arguments\nUsage - ./ircserv <port> <password>" << std::endl;
		return (1);
	} else if (stringToInt(argv[1]) < 1024 || stringToInt(argv[1]) > 65535) {
		std::cerr << "Invalid port range\nUse values in range of 1024 to 65535" << std::endl;
		return (1);
	}
	signal(SIGINT, handleSignal);
	globalServer = new Server(stringToInt(argv[1]), argv[2]);
	globalServer->start();
	delete globalServer;
	return (0);
}
