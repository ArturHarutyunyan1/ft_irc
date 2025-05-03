#include "../../include/Requests.hpp"

void Requests::TOPIC(const std::string &channelName, const std::string &topic) {
	Channel *channel = _server.getChannel(channelName);

	if (channel) {
		if (channel->getTopicSettableByOp()) {
			if (channel->isOperator(_client.getNick())) {
				channel->setTopic(topic);
				sendToEveryone(*channel, yellow + serverName + " TOPIC " + channelName + " :" + topic + reset + "\n");
			} else
				sendSystemMessage(this->_fd, red + serverName + " 482 :You must be operator" + reset + "\n");
		} else {
			channel->setTopic(topic);
			sendToEveryone(*channel, yellow + serverName + " TOPIC " + channelName + " :" + topic + reset + "\n");
		}
	} else
		sendSystemMessage(this->_fd, red + serverName + ": 412 " + _client.getNick() + " :No such channel " + channelName + reset + "\n");
}