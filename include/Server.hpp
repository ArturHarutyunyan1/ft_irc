#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Server
{
    private:
        int _port;
        std::string _password;

    public:
        Server(int port, std::string password);
        ~Server();
        void start();
        int getPort() const;
        std::string getPassword() const;
};

#endif