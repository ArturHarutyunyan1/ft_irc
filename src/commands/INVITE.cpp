#include "../../include/Requests.hpp"

void Requests::INVITE(const std::string &nickname, const std::string &channelName) {
	Channel *channel = _server.getChannel(channelName);

	if (channel) {
		int user = _server.getUser(nickname);

		if (user != -1 && !channel->isClient(nickname)) {
			if (channel->isClient(_client.getNick())) {
				channel->inviteClient(nickname);
				sendSystemMessage(_server.getUser(nickname), green + serverName + " 341 " + _client.getNick() + " invited " + nickname + " to :" + channelName + reset + "\n");
			} else
				sendSystemMessage(this->_fd, red + serverName + " 401 :You are not in channel " + channelName + reset + "\n");
		} else if (channel->isClient(nickname)) {
			sendSystemMessage(this->_fd, red + serverName + " 401 :User is already in channel " + channelName + reset + "\n");
		} else {
			sendSystemMessage(this->_fd, red + serverName + " 401 " + _client.getNick() + " " + nickname + " :No such nickname" + reset + "\n");
		}
	} else {
		sendSystemMessage(this->_fd, red + serverName + " 401 " + _client.getNick() + " " + channelName + " :No such channel" + reset + "\n");
	}
}