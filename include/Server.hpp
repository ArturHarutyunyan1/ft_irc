#ifndef SERVER_HPP
#define SERVER_HPP

# include "Client.hpp"
# include "Utils.hpp"
# include "Channel.hpp"
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
# include <netinet/in.h>

#define MAX_CONNECTIONS 100

class Client;
class Channel;

class Server
{
    private:
        int _port;
        std::string _host;
        std::string _password;
        struct pollfd   *_client_fds;
        // Map of channels
        std::map<std::string, Channel*> _channels;
        // Map of users and their fds
        std::map<std::string, int> _usernameToFd;
        // Map of fd to Client pointer
        std::map<int, Client*> _clients;
        // Map of fd to username
        std::map<int, std::string> _fdToUsername;
    public:
        Server(int port, std::string password);
        ~Server();
        void start();
        void newClient(int fd);
        void handleRequest(int i);
        int getPort() const;
        void addUser(const std::string &nickname, int fd);
        void addFd(int fd, const std::string &username);
        int getUser(const std::string &nickname) const;
        std::string getPassword() const;
        std::string getNick(int fd) const;
        void removeUser(const std::string &nickname, int fd);
        Channel *getChannel(const std::string &channelName);
        void addChannel(const std::string &channelName, Channel *channel);
};

#endif