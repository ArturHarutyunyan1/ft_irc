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
        std::string _nick;
        std::string _username;
        std::string _realname;
        std::string _ip;
        bool _authenticationStatus;
    public:
        int getFd() const;
        std::string getNick() const;
        std::string getUsername() const;
        std::string getRealname() const;
        std::string getIP() const;
        bool isAuthenticated() const;

        void setNick(const std::string &name);
        void setUsername(const std::string &name, const std::string &realname);
        void setAuthStatus(bool status);
        void setPassword(const std::string &password);
        void setIP(const std::string &ip);

        bool isPasswordSet() const;
        bool isNickSet() const;
        bool isUserSet() const;


        Client(int fd, bool status);
        ~Client();
};

#endif