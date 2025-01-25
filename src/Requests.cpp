#include "../include/Requests.hpp"

Requests::Requests(char *msg, struct pollfd *fds, int fd, std::string _password, bool isSet) : _password(_password), _message(msg), _fd(fd), _fds(fds), _isSet(isSet)
{
}

void Requests::handleRequest()
{
    std::string message(_message);
    std::string response;
    if (message == "HELP\n")
        response = helpMessage();
    else if (std::strncmp(_message, "PASS", 4) == 0)
        response = PASS(message);
    send(this->_fds[this->_fd].fd, response.c_str(), response.size(), 0);
}

std::string Requests::PASS(std::string msg)
{
    std::string pass;
    size_t pos = msg.find(' ');
    
    if (pos != std::string::npos)
        pass = msg.substr(pos + 1);
    size_t end = pass.size();
    while (end > 0 && (pass[end - 1] == ' ' || pass[end - 1] == '\n' || pass[end - 1] == '\r'))
        --end;
    pass = pass.substr(0, end);
    
    if (pass == _password && !_isSet)
    {
        _isSet = true;
        return ("Password was set successfully!\n");
    }
    else
        return ("Invalid password!\n");
}