#include "../include/Server.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    Server server = Server(std::stoi(argv[1]), argv[2]);

    server.start();
    return (0);
}