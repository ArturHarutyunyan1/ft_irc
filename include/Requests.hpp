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
        std::string _password;
        char *_message;
        int _fd;
        struct pollfd *_fds;
        bool _isSet;
    public:
        Requests(char *msg, struct pollfd *fds, int fd, std::string _password, bool isSet);
        // Requests(const Requests &copy);
        // Requests &operator=(const Requests &copy);
        // ~Requests();
        void handleRequest();
        std::string PASS(std::string msg);
};

#endif