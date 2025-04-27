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
# include <map>
# include <stdlib.h>
# include <ctime>
# include <errno.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <sys/poll.h>
# include <netdb.h>
# include <netinet/in.h>
# include <openssl/ssl.h>
# include <openssl/err.h>

#define MAX_CONNECTIONS 100

class Client;

class Server
{
    private:
        int _port;
        std::string _password;
        struct pollfd   *_client_fds;
        // Map of channels
        // Map of users and their fds
        std::map<std::string, int> _users;
    public:
        Server(int port, std::string password);
        ~Server();
        void start();
        void newClient(int fd);
        void handleRequest(int i);
        int getPort() const;
        void addUser(const std::string &nickname, int fd);
        int getUser(const std::string &nickname) const;
        std::string getPassword() const;

        static void getResponseFromBot(std::string msg);
        std::string getNick(int fd) const;
};

#endif