#include "../../include/Requests.hpp"

void Requests::TOPIC(const std::string &channelName, const std::string &topic) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        if (channel->getTopicSettableByOp()) {
            if (channel->isOperator(_server->getNick(this->_fd))) {
                channel->setTopic(topic);
                sendToEveryone(channel, "Channel topic was set to " + topic + "\n");
            } else
                sendSystemMessage(this->_fd, "Only operator can set topic\n");
        } else {
            channel->setTopic(topic);
            sendToEveryone(channel, "Channel topic was set to " + topic + "\n");
        }
    } else
        sendSystemMessage(this->_fd, "No such channel " + channelName + "\n");
}