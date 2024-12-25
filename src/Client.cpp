#include "../include/Client.hpp"

Client::Client(int fd, std::string password) : _fd(fd), _password(password)
{
    this->_nickname = "";
    this->_username = "";
    this->_authenticated = false;
}

int Client::getFd() const
{
    return (this->_fd);
}

bool Client::isAuthenticated() const
{
    return (this->_authenticated);
}

void Client::authenticate(std::string password)
{
    if (password != _password)
    {
        send(_fd, "Authentication failed\n", 22, 0);
        close (_fd);
    }
    else
    {
        _authenticated = true;
        send(_fd, "Successfully authenticated\n", 27, 0);
    }
}

void Client::setNickname(std::string nick)
{
    this->_nickname = nick;
}

void Client::setUsername(std::string user)
{
    this->_username = user;
}

void Client::handleCommunication(int fd)
{
    while (true)
    {
        std::string response;
        std::cout << "Enter message: ";
        std::getline(std::cin, response);
        if (response == "quit")
        {
            std::cout << "Closing communication." << std::endl;
            break;
        }
        send(fd, response.c_str(), response.size(), 0);
    }
}

