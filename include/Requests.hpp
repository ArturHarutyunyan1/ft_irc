#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include "Client.hpp"
#include "Server.hpp"
#include "Utils.hpp"

class Client;
class Server;

class Requests
{
    private:
        char *_message;
        int _fd;
        struct pollfd *_fds;
    public:
        Requests(char *msg, struct pollfd *fds, int fd);
        // Requests(const Requests &copy);
        // Requests &operator=(const Requests &copy);
        // ~Requests();
        void handleRequest();
};

#endif