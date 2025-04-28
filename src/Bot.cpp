#include "../include/Bot.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/err.h>

Bot::Bot() : _state(IDLE)
{
    this->initSocket();
}

Bot::~Bot()
{
    this->cleanup();
}

void Bot::initSocket()
{
    this->_socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (this->_socketFd < 0)
    {
        perror("Bot socket creation failed");
        return;
    }
}

void Bot::cleanup()
{
    close(this->_socketFd);
}

int Bot::getSocketFd()
{
    return this->_socketFd;
}

State   Bot::getState()
{
    return this->_state;
}