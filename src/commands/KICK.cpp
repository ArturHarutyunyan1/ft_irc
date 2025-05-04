#include "../../include/Requests.hpp"

void Requests::KICK(const std::string &channelName, const std::string &nickname, const std::string &reason) {
    Channel *channel = _server.getChannel(channelName);

    if (!channel) {
        sendSystemMessage(this->_fd, serverName + " 403 " + _client.getNick() + " " + channelName + " :No such channel\n");
        return;
    }

    if (!channel->isClient(_client.getNick())) {
        sendSystemMessage(this->_fd, serverName + " 442 " + _client.getNick() + " " + channelName + " :You're not on that channel\n");
        return;
    }

    if (!channel->isOperator(_client.getNick())) {
        sendSystemMessage(this->_fd, serverName + " 482 " + _client.getNick() + " " + channelName + " :You're not a channel operator\n");
        return;
    }

    if (_client.getNick() == nickname) {
        sendSystemMessage(this->_fd, serverName + " 482 " + _client.getNick() + " " + channelName + " :You can't kick yourself\n");
        return;
    }

    if (_server.getUser(nickname) != -1 && channel->isClient(nickname)) {
        std::string kickMessage = ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP() +
                                  " KICK " + channelName + " " + nickname;

        if (!reason.empty())
            kickMessage += " :" + reason;

        kickMessage += "\n";

        sendToEveryone(*channel, kickMessage);
        channel->kickClient(nickname);
    } else {
        sendSystemMessage(this->_fd, serverName + " 401 " + _client.getNick() + " " + nickname + " :No such nick\n");
    }
}