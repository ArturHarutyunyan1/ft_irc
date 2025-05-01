#include "../../include/Requests.hpp"

void Requests::KICK(const std::string &channelName, const std::string &nickname) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        if (channel->isOperator(_server->getNick(this->_fd))) {
            if (_server->getNick(this->_fd) != nickname) {
                sendToEveryone(channel, _server->getNick(this->_fd) + " kicked " + nickname + " from channel " + channelName + "\n");
                channel->kickClient(nickname);
            } else {
                sendSystemMessage(this->_fd, "You can't kick yourself\n");
            }
        } else
            sendSystemMessage(this->_fd, "You are not operator\n");
    } else
        sendSystemMessage(this->_fd, "No such channel " + channelName + "\n");
}