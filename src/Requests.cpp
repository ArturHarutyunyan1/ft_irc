#include "../include/Requests.hpp"

Requests::Requests(char *msg, struct pollfd *fds, int fd) : _message(msg), _fd(fd), _fds(fds)
{
}

void Requests::handleRequest()
{
    std::string message(_message);
    std::string response;

    if (message == "HELP\n")
    {
        response = helpMessage();
        send(this->_fds[this->_fd].fd, response.c_str(), response.size(), 0);
    }
}