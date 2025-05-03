#include "../../include/Requests.hpp"

void Requests::INVITE(const std::string &nickname, const std::string &channelName) {
	Channel *channel = _server.getChannel(channelName);

	if (!channel) {
		sendSystemMessage(this->_fd, serverName + " 403 " + _client.getNick() + " " + channelName + " :No such channel\n");
		return;
	}

	int userFD = _server.getUser(nickname);
	if (userFD == -1) {
		sendSystemMessage(this->_fd, serverName + " 401 " + _client.getNick() + " " + nickname + " :No such nick\n");
		return;
	}

	if (!channel->isClient(_client.getNick())) {
		sendSystemMessage(this->_fd, serverName + " 442 " + _client.getNick() + " " + channelName + " :You're not on that channel\n");
		return;
	}

	if (channel->isClient(nickname)) {
		sendSystemMessage(this->_fd, serverName + " 443 " + _client.getNick() + " " + nickname + " " + channelName + " :is already on channel\n");
		return;
	}

	channel->inviteClient(nickname);
	sendSystemMessage(this->_fd, serverName + " 341 " + _client.getNick() + " " + nickname + " " + channelName + "\n");
	std::string prefix = ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP();
	sendSystemMessage(userFD, prefix + " INVITE " + nickname + " :" + channelName + "\n");
}
