#include <algorithm>

#include "../include/Channel.hpp"

Channel::Channel(void) throw() : clientLimit(-1), isInviteOnly(false), isTopicSettableByOp(true) {
}

Channel::Channel(std::string const& creator) throw(std::bad_alloc)
: clientLimit(-1), isInviteOnly(false),
	isTopicSettableByOp(true) {
	clients.insert(creator);
	ops.insert(creator);
}

Channel::~Channel(void) throw() {
}

Channel::Channel(Channel const& other) throw(std::bad_alloc)
: clients(other.clients), invited(other.invited), ops(other.ops), topic(other.topic),
	key(other.key), clientLimit(other.clientLimit), isInviteOnly(other.isInviteOnly),
	isTopicSettableByOp(other.isTopicSettableByOp) {
}

Channel&	Channel::operator=(Channel const& other) throw(std::bad_alloc) {
	if (this != &other) {
		clients = other.clients;
		invited = other.invited;
		ops = other.ops;
		topic = other.topic;
		key = other.key;
		clientLimit = other.clientLimit;
		isInviteOnly = other.isInviteOnly;
		isTopicSettableByOp = other.isTopicSettableByOp;
	}
	return *this;
}

ChannelClientStatus Channel::addClient(std::string const& nickname, std::string const& key)
throw(std::bad_alloc) {
    ChannelClientStatus status;

    if (clients.find(nickname) != clients.end())
        status = CHANNEL_CLIENT_ALREADY_IN;
    else if (isInviteOnly) {
        std::set<std::string>::iterator invitation;
        if ((invitation = invited.find(nickname)) == invited.end())
            status = CHANNEL_CLIENT_NOT_INVITED;
        else {
            status = CHANNEL_OK;
            invited.erase(invitation);
        }
    } else if (clientLimit != -1 && clients.size() >= static_cast<size_t>(clientLimit)) // not compiling without cast
        status = CHANNEL_NOT_ENOUGH_PLACES;
    else if (!this->key.empty() && this->key != key)
        status = CHANNEL_INVALID_KEY;
    else
        status = CHANNEL_OK;
	// Need to insert client here
	if (status == CHANNEL_OK)
        clients.insert(nickname);
    return status;
}


void	Channel::kickClient(std::string const& nickname) throw() {
	clients.erase(nickname);
	ops.erase(nickname);
}

void	Channel::changeClientNickname(std::string const& old, std::string const& newNick)
throw(std::bad_alloc) {
	if (clients.erase(old))
		clients.insert(newNick);
	if (invited.erase(old))
		invited.insert(newNick);
	if (ops.erase(old))
		ops.insert(newNick);
}

bool	Channel::isOperator(std::string const& nickname) const throw() {
	return ops.find(nickname) != ops.end();
}

bool	Channel::isClient(std::string const& nickname) const throw() {
	return clients.find(nickname) != clients.end();
}

void	Channel::inviteClient(std::string const& nickname) throw(std::bad_alloc) {
	invited.insert(nickname);
}

void	Channel::setTopic(std::string const& topic) throw(std::bad_alloc) {
	this->topic = topic;
}

std::string const&	Channel::getTopic(void) const throw() {
	return topic;
}

void	Channel::setInviteOnly(bool isInviteOnly) throw() {
	this->isInviteOnly = isInviteOnly;
}

bool	Channel::getInviteOnly(void) const throw() {
	return isInviteOnly;
}

void	Channel::setTopicSettableByOp(bool isTopicSettableByOp) throw() {
	this->isTopicSettableByOp = isTopicSettableByOp;
}

bool	Channel::getTopicSettableByOp(void) const throw() {
	return isTopicSettableByOp;
}

void	Channel::setKey(std::string const& key) throw(std::bad_alloc) {
	this->key = key;
}

std::string const&	Channel::getKey(void) const throw() {
	return key;
}

void	Channel::addOperator(std::string const& nickname) throw(std::bad_alloc) {
	ops.insert(nickname);
}

void	Channel::removeOperator(std::string const& nickname) throw() {
	ops.erase(nickname);
}

void	Channel::setClientLimit(int limit) throw() {
	clientLimit = limit;
}

std::set<std::string>  Channel::getClients(void) const throw() {
    return clients;
}
