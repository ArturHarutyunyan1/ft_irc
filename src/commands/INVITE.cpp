#include "../../include/Requests.hpp"

void Requests::INVITE(const std::string &channelName, const std::string &nickname) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        int user = _server->getUser(nickname);

        if (user != -1) {
            channel->inviteClient(nickname);
            sendSystemMessage(_server->getUser(nickname), green + serverName + " 341 " + _client->getNick() + " invited " + nickname + " to :" + channelName + reset + "\n");
        } else {
            sendSystemMessage(this->_fd, red + serverName + " 401 " + _client->getNick() + " " + nickname + " :No such nickname" + reset + "\n");
        }
    } else {
        sendSystemMessage(this->_fd, red + serverName + " 401 " + _client->getNick() + " " + nickname + " :No such channel" + reset + "\n");
    }
}