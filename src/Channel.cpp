#include <algorithm>

#include "../include/Channel.hpp"

Channel::Channel(void) throw() : clientLimit(-1), isInviteOnly(false), isTopicSettableByOp(true) {
}

Channel::Channel(std::string const& creator) throw(std::bad_alloc)
: clients(1, creator), ops(1, creator), clientLimit(-1), isInviteOnly(false),
	isTopicSettableByOp(true) {
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

ChannelClientStatus	Channel::addClient(std::string const& nickname, std::string const& key) throw(std::bad_alloc) {
	ChannelClientStatus	status;

	if (std::find(clients.begin(), clients.end(), nickname) != clients.end())
		status = CHANNEL_CLIENT_ALREADY_IN;
	else if (isInviteOnly) {
		std::list<std::string>::iterator	invitation;

		if ((invitation = std::find(invited.begin(), invited.end(), nickname)) == invited.end())
			status = CHANNEL_CLIENT_NOT_INVITED;
		else {
			status = CHANNEL_OK;
			invited.erase(invitation);
		}
	} else if (clients.size() == clientLimit)
		status = CHANNEL_NOT_ENOUGH_PLACES;
	else if (!this->key.empty() && this->key != key)
		status = CHANNEL_INVALID_KEY;
	else
		status = CHANNEL_OK;
	return status;
}

void	Channel::kickClient(std::string const& nickname) throw() {
	std::list<std::string>::iterator	clientToErase = std::find(clients.begin(), clients.end(),
		nickname);
	std::list<std::string>::iterator	operatorToErase = std::find(ops.begin(), ops.end(),
		nickname);
	if (clientToErase != clients.end())
		clients.erase(clientToErase);
	if (operatorToErase != ops.end())
		ops.erase(operatorToErase);
}

bool	Channel::isOperator(std::string const& nickname) const throw() {
	return std::find(ops.begin(), ops.end(), nickname) != ops.end();
}

bool	Channel::isClient(std::string const& nickname) const throw() {
	return std::find(clients.begin(), clients.end(), nickname) != clients.end();
}
