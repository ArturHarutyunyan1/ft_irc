#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Server.hpp"
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

class Server;

class Client
{
    private:
        int _fd;
        std::string _password;
        std::string _nickname;
        std::string _username;
        bool _authenticated;
    public:
        Client(int fd, std::string password);
        int getFd() const;
        bool isAuthenticated() const;
        void authenticate(std::string password);
        void setNickname(std::string nickname);
        void setUsername(std::string username);
        void handleCommunication(int fd);

};

#endif