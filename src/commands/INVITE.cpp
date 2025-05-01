#include "../../include/Requests.hpp"

void Requests::INVITE(const std::string &channelName, const std::string &nickname) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        int user = _server->getUser(nickname);

        if (user != -1) {
            channel->inviteClient(nickname);
            sendSystemMessage(_server->getUser(nickname), _server->getNick(this->_fd) + " invited you to channel " + channelName + "\n");
        } else {
            sendSystemMessage(this->_fd, "No such user " + nickname + "\n");
        }
    } else {
        sendSystemMessage(this->_fd, "No such channel " + channelName + "\n");
    }
}