#include "../include/Client.hpp"

Client::Client(std::string const& ip) throw(std::bad_alloc) : _ip(ip) {}

Client::Client(void) throw(std::bad_alloc) {
}

Client::~Client() {}

Client::Client(const Client &other)
	: _password(other._password), _nick(other._nick),
	  _username(other._username), _realname(other._realname),
	  _ip(other._ip)
{
}

Client &Client::operator=(const Client &other) {
	if (this != &other) {
		_password = other._password;
		_nick = other._nick;
		_username = other._username;
		_realname = other._realname;
		_ip = other._ip;
	}
	return *this;
}

bool	Client::operator==(Client const& other) const throw() {
	return _nick == other._nick;
}

bool	Client::operator<(Client const& other) const throw() {
	return _nick < other._nick;
}

std::string const& Client::getNick() const {
	return (this->_nick);
}

std::string const& Client::getUsername() const {
	return (this->_username);
}

std::string const& Client::getRealname() const {
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

void Client::setPassword(const std::string &password) {
	this->_password = password;
}

bool Client::isPasswordSet() const {
	return !this->_password.empty();
}

bool Client::isNickSet() const {
	return !this->_nick.empty();
}

bool Client::isUserSet() const {
	return !this->_username.empty();
}

std::string const& Client::getIP() const {
	return (this->_ip);
}

std::set<std::string> const& Client::getChannels() const {
	return (this->_channels);
}

void Client::addChannel(const std::string &channelName) {
	this->_channels.insert(channelName);
}

void Client::removeChannel(const std::string &channel) {
	std::set<std::string>::iterator it = _channels.find(channel);

	if (it != _channels.end())
		_channels.erase(it);
}