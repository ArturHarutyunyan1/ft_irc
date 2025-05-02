#include "../../include/Requests.hpp"

void Requests::KICK(const std::string &channelName, const std::string &nickname) {
	Channel *channel = _server.getChannel(channelName);

	if (channel) {
		if (channel->isOperator(_client.getNick())) {
			if (_client.getNick() != nickname) {
				if (_server.getUser(nickname) != -1 && channel->isClient(nickname)) {
					sendToEveryone(*channel, yellow + serverName + " " + _client.getNick() + "@" + _client.getUsername() + "!" + _client.getIP() + " KICK " + channelName + " " + nickname + reset + "\n");
					channel->kickClient(nickname);
				} else {
					sendSystemMessage(this->_fd, red + serverName + ": 412 " + _client.getNick() + " :No such user " + nickname + reset + "\n");
				}                
			} else {
				sendSystemMessage(this->_fd, red + serverName + " :You can't kick yourself" + reset + "\n");
			}
		} else
			sendSystemMessage(this->_fd, red + serverName + " 482 :You must be operator" + reset + "\n");
	} else
		sendSystemMessage(this->_fd, red + serverName + ": 412 " + _client.getNick() + " :No such channel " + channelName + reset + "\n");
}