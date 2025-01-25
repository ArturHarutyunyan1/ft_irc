#ifndef SERVER_HPP
#define SERVER_HPP

# include "Client.hpp"
# include "Utils.hpp"
# include "Requests.hpp"
# include <vector>
# include <iostream>
# include <stdio.h>
# include <string>
# include <fcntl.h>
# include <stdlib.h>
# include <ctime>
# include <errno.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <sys/poll.h>
# include <netinet/in.h>

#define MAX_CONNECTIONS 100

class Client;

class Server
{
    private:
        int _port;
        std::string _password;
        struct pollfd   *_client_fds;
    public:
        Server(int port, std::string password);
        ~Server();
        void start();
        void newClient(int fd);
        void handleRequest(int i);
        int getPort() const;
        std::string getPassword() const;
};

#endif