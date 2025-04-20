#include "../include/Server.hpp"

int main(int argc, char **argv)
{
    (void)argc;
    Server server = Server(string_to_number(argv[1]), argv[2]);

    server.start();
    return (0);
}