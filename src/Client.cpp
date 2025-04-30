#include "../include/Client.hpp"

Client::Client(int fd, bool status) : _fd(fd), _authenticationStatus(status) {}

Client::~Client() {}

Client::Client(const Client &other)
    : _fd(other._fd), _password(other._password), _nick(other._nick),
      _username(other._username), _realname(other._realname),
      _ip(other._ip), _authenticationStatus(other._authenticationStatus)
{
}

Client &Client::operator=(const Client &other)
{
    if (this != &other)
    {
        _fd = other._fd;
        _password = other._password;
        _nick = other._nick;
        _username = other._username;
        _realname = other._realname;
        _ip = other._ip;
        _authenticationStatus = other._authenticationStatus;
    }
    return *this;
}

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

void Client::setIP(const std::string &ip) {
    this->_ip = ip;
}

std::string Client::getIP() const {
    return (this->_ip);
}

std::set<std::string> Client::getChannels() const {
    return (this->_channels);
}

void Client::addChannel(const std::string &channelName) {
    this->_channels.insert(channelName);
}
