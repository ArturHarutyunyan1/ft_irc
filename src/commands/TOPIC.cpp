#include "../../include/Requests.hpp"

void Requests::TOPIC(const std::string &channelName, const std::string &topic) {
    Channel *channel = _server.getChannel(channelName);

    if (channel) {
        if (!channel->isClient(_client.getNick())) {
            sendSystemMessage(this->_fd, serverName + " 442 :You're not in channel " + channelName + "\n");
            return;
        }
        if (channel->getTopicSettableByOp() && !channel->isOperator(_client.getNick())) {
            sendSystemMessage(this->_fd, serverName + " 482 :You must be operator" + "\n");
            return;
        }
        channel->setTopic(topic);
        sendToEveryone(*channel, serverName + " TOPIC " + channelName + " :" + topic + "\n");
    } else {
        sendSystemMessage(this->_fd, serverName + ": 412 " + _client.getNick() + " :No such channel " + channelName + "\n");
    }
}
