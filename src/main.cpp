#include "../include/Server.hpp"

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "Invalid arguments\nUsage - ./ircserv <port> <password>" << std::endl;
        return (1);
    } else if (stringToInt(argv[1]) < 1024 || stringToInt(argv[1]) > 65535) {
        std::cerr << "Invalid port range\nUse values in range of 1024 to 65535" << std::endl;
        return (1);
    }
    
    Server server = Server(stringToInt(argv[1]), argv[2]);

    server.start();
    return (0);
}