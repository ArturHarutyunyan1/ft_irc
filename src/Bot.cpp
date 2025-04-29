#include "../include/Bot.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/err.h>
#include <iostream>

Bot::Bot()
{
    initSocket();
}

Bot::~Bot()
{
    cleanup();
}

void Bot::initSocket()
{
    this->setState(DEFAULT);
    this->setSslCtx(NULL);
    this->setSsl(NULL);
    this->setClientFd(-1);

    this->_socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (this->getSocketFd() < 0)
    {
        perror("socket");
        // throw
        return;
    }
}

void Bot::cleanup()
{
    if (this->getSsl())
    {
        SSL_shutdown(this->_ssl);
        SSL_free(this->_ssl);
        this->setSsl(NULL);
    }
    if (this->getSslCtx())
    {
        SSL_CTX_free(this->_sslCtx);
        this->setSslCtx(NULL);
    }
    if (this->getSocketFd() >= 0)
        close(this->_socketFd);

    this->setRequest("");
    this->setResponse("");
}

int Bot::getSocketFd()
{
    return this->_socketFd;
}

void Bot::setSocketFd(int fd)
{
    this->_socketFd = fd;
}

State Bot::getState()
{
    return this->_state;
}

void Bot::setState(State state)
{
    this->_state = state;
}

SSL_CTX* Bot::getSslCtx()
{
    return this->_sslCtx;
}

void Bot::setSslCtx(SSL_CTX* ctx)
{
    this->_sslCtx = ctx;
}

SSL* Bot::getSsl()
{
    return this->_ssl;
}

void Bot::setSsl(SSL* ssl)
{
    this->_ssl = ssl;
}

std::string& Bot::getRequest()
{
    return this->_request;
}

void Bot::setRequest(std::string req)
{
    this->_request = req;
}

std::string& Bot::getResponse()
{
    return this->_response;
}

void Bot::setResponse(std::string resp)
{
    this->_response = resp;
}

void Bot::setClientFd(int fd)
{
    this->_clientFd = fd;
}

int Bot::getClientFd()
{
    return this->_clientFd;
}