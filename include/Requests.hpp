#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <iostream>
#include <sstream>
#include <string>
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
        Server *_server;
    public:
        Requests(char *msg, struct pollfd *fds, int fd, std::string _password, bool isSet, Server *_server);
        // Requests(const Requests &copy);
        // Requests &operator=(const Requests &copy);
        // ~Requests();
        void handleRequest();
        std::string PASS(std::string msg);
        std::string NICK(const std::string &nickname);
        std::string JOIN(const std::string &channel, const std::string &key);
        void PRIVMSG(const std::string &receiver, const std::string &message) const;
        void KICK(const std::string &channel, const std::string &nickname);
};

#endif