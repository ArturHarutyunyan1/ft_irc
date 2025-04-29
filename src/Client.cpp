#include "../include/Client.hpp"

Client::Client(int fd, bool status) : _fd(fd), _authenticationStatus(status) {}

Client::~Client() {}

int Client::getFd() const {
    return (this->_fd);
}

std::string Client::getNick() const {
    return (this->_nick);
}

std::string Client::getUsername() const {
    return (this->_username);
}

std::string Client::getRealname() const {
    return (this->_realname);
}

bool Client::isAuthenticated() const {
    return (isUserSet() && isNickSet() && isPasswordSet());
}

void Client::setNick(const std::string &nick) {
    this->_nick = nick;
}

void Client::setUsername(const std::string &username, const std::string &realname) {
    this->_username = username;
    this->_realname = realname;
}


void Client::setAuthStatus(bool status) {
    this->_authenticationStatus = status;
}

void Client::setPassword(const std::string &password) {
    this->_password = password;
}

bool Client::isPasswordSet() const {
    if (!this->_password.empty())
        return (true);
    return (false);
}

bool Client::isNickSet() const {
    if (!this->_nick.empty())
        return (true);
    return (false);
}

bool Client::isUserSet() const {
    if (!this->_username.empty())
        return (true);
    return (false);
}