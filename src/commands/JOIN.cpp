#include "../../include/Requests.hpp"

void Requests::JOIN(const std::string &channelName, const std::string &key) {
	Channel *channel = _server.getChannel(channelName);
	std::string const& nickname = _client.getNick();

	if (!_client.isNickSet())
		sendSystemMessage(this->_fd, serverName + " 451 " + nickname + " JOIN :You have not registered\n");
	if (!channel) {
		Channel newChannel = Channel(nickname);
		_server.addChannel(channelName, newChannel);
		_client.addChannel(channelName);
		channel = _server.getChannel(channelName);
		sendToEveryone(*channel, ":" + nickname + "!" + _client.getUsername() + "@" + _client.getIP() + " JOIN " + channelName + "\n");
		return;
	}
	ChannelClientStatus status = channel->addClient(nickname, key);

	if (status == CHANNEL_CLIENT_ALREADY_IN) {
		sendSystemMessage(this->_fd, serverName + " 443 " + nickname + " " + channelName + " :is already on channel\n");
	}
	else if (status == CHANNEL_CLIENT_NOT_INVITED) {
		sendSystemMessage(this->_fd, serverName + " 473 " + nickname + " " + channelName + " :Cannot join channel (+i)\n");
	}
	else if (status == CHANNEL_NOT_ENOUGH_PLACES) {
		sendSystemMessage(this->_fd, serverName + " 471 " + nickname + " " + channelName + " :Cannot join channel (+l)\n");
	}
	else if (status == CHANNEL_INVALID_KEY) {
		sendSystemMessage(this->_fd, serverName + " 475 " + nickname + " " + channelName + " :Cannot join channel (+k)\n");
	}
	else
	{
		sendToEveryone(*channel, ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP() + " JOIN " + channelName + "\n");
		if (channel->getTopic().size() > 0)
			sendSystemMessage(this->_fd,  serverName + " 332 " + nickname + " " + channelName + " :" + channel->getTopic() + "\n");
		else
			sendSystemMessage(this->_fd,  serverName + " 332 " + nickname + " " + channelName + " :No topic\n");
		
		std::set<std::string> clients = channel->getClients();
		std::string namesLine;
		for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it) {
			namesLine += *it + " ";
		}
		if (!namesLine.empty())
			namesLine.erase(namesLine.length() - 1);

		sendSystemMessage(this->_fd, namesLine + "\n");
		sendSystemMessage(this->_fd, serverName + " 366 " + nickname + " " + channelName + " :End of NAMES list\n");
	}
}
