#include "../include/Bot.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

Bot::Bot()
{
    initSocket();
}

Bot::Bot(const Bot& bot)
{
    this->setClientFd(bot._clientFd);
    this->_state = bot._state;
    this->_request = bot._request;
    this->_response = bot._response;
    this->_ssl = bot._ssl;
    this->_sslCtx = bot._sslCtx;
}

Bot&    Bot::operator=(const Bot& bot)
{
    if (this != &bot)
    {
        this->setClientFd(bot._clientFd);
        this->_state = bot._state;
        this->_request = bot._request;
        this->_response = bot._response;
        this->_ssl = bot._ssl;
        this->_sslCtx = bot._sslCtx;
    }

    return *this;
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

void Bot::setRequest(std::string& req)
{
    this->_request = req;
}

std::string& Bot::getResponse()
{
    return this->_response;
}

void Bot::setResponse(std::string& resp)
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