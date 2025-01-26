#include "../include/Requests.hpp"

Requests::Requests(char *msg, struct pollfd *fds, int fd, std::string _password, bool isSet) : _password(_password), _message(msg), _fd(fd), _fds(fds), _isSet(isSet)
{
}

void Requests::handleRequest()
{
    std::string message(_message);
    std::string response;
    std::string value;
    size_t pos = message.find(' ');
    
    if (pos != std::string::npos)
        value = message.substr(pos + 1);
    size_t end = value.size();
    while (end > 0 && (value[end - 1] == ' ' || value[end - 1] == '\n' || value[end - 1] == '\r'))
        --end;
    value = value.substr(0, end);
    if (message == "HELP\n")
        response = helpMessage();
    else if (std::strncmp(_message, "PASS", 4) == 0)
        response = PASS(value);
    else
        response = "Unknown command\n";
    send(this->_fds[this->_fd].fd, response.c_str(), response.size(), 0);
}

std::string Requests::PASS(std::string msg)
{
    if (msg == _password && !_isSet)
    {
        _isSet = true;
        return ("Password was set successfully!\n");
    }
    else
        return ("Invalid password!\n");
}