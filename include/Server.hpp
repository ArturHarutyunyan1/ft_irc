#ifndef SERVER_HPP
#define SERVER_HPP

# include "Client.hpp"
# include "Utils.hpp"
# include "Channel.hpp"
# include "Bot.hpp"
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

#define MAX_CONNECTIONS 100

class Client;
class Channel;

class Server
{
    private:
        int _port;
        std::string _password;
        struct pollfd   *_client_fds;
        std::map<int, Bot> _botRequests;
        // Map of channels
        // std::map<std::string, Channel> _channels;
        // Map of users and their fds
        std::map<std::string, int> _usernameToFd;
        std::map<int, std::string> _fdToUsername;
    public:
        Server(int port, std::string password);
        ~Server();
        void start();
        void newClient(int fd);
        void handleRequest(int i);
        void handleRequestBot(int i);
        int getPort() const;
        void addUser(const std::string &nickname, int fd);
        void addFd(int fd, const std::string &username);
        int getUser(const std::string &nickname) const;
        // bool joinChannel(const std::string &channel, const std::string &nickname);
        std::string getPassword() const;

        std::string getNick(int fd) const;
        void removeUser(const std::string &nickname, int fd);

        int findFreeFdSlot();

        Bot* findBotBySocket(int socketFd);
        void sendRequestToBot(std::string msg, int clientFd);
        void cleanupBot(Bot* bot, int pollfdIndex);
        
        void botInit(Bot *bot, int idx);
        void botHandshake(Bot *bot, int idx);
        void botSending(Bot *bot, int idx);
        void botReceiving(Bot *bot, int idx);
};

#endif