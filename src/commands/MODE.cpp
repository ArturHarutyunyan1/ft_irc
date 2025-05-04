#include "../../include/Requests.hpp"

void Requests::MODE(Channel& channel, const std::string &channelName, const std::string &flag, const std::string &extra) {
    if (!channel.isOperator(_client.getNick())) {
        sendSystemMessage(this->_fd, serverName + " 482 " + _client.getNick() + " " + channelName + " :You're not a channel operator\n");
        return;
    }

    if (flag == "+i") {
        channel.setInviteOnly(true);
        sendToEveryone(channel, serverName + " 473 " + _client.getNick() + " " + channelName + " :Channel is now invite-only\n");
    } else if (flag == "-i") {
        channel.setInviteOnly(false);
        sendToEveryone(channel, serverName + " 473 " + _client.getNick() + " " + channelName + " :Channel is no longer invite-only\n");
    } else if (flag == "+t") {
        channel.setTopicSettableByOp(true);
        sendToEveryone(channel,  serverName + " 332 " + _client.getNick() + " " + channelName + " :Only operators can set the topic\n");
    } else if (flag == "-t") {
        channel.setTopicSettableByOp(false);
        sendToEveryone(channel,  serverName + " 332 " + _client.getNick() + " " + channelName + " :All users can set the topic\n");
    } else if (flag == "+k") {
        channel.setKey(extra);
        sendToEveryone(channel,  serverName + " 475 " + _client.getNick() + " " + channelName + " :Channel now has a password\n");
    } else if (flag == "-k") {
        channel.setKey("");
        sendToEveryone(channel,  serverName + " 475 " + _client.getNick() + " " + channelName + " :Channel password removed\n");
    } else if (flag == "+o") {
        if (_server.getUser(extra) != -1 && channel.isClient(extra)) {
            channel.addOperator(extra);
            sendToEveryone(channel, serverName + " 381 " + _client.getNick() + " " + extra + " :is now a channel operator\n");
        } else {
            sendSystemMessage(this->_fd, serverName + " 401 " + _client.getNick() + " " + extra + " :No such user\n");
        }
    } else if (flag == "-o") {
        if (_server.getUser(extra) != -1) {
            channel.removeOperator(extra);
            sendToEveryone(channel,  serverName + " 381 " + _client.getNick() + " " + extra + " :is no longer a channel operator\n");
        } else {
            sendSystemMessage(this->_fd, serverName + " 401 " + _client.getNick() + " " + extra + " :No such user\n");
        }
    } else if (flag == "+l") {
        channel.setClientLimit(stringToInt(extra));
        sendToEveryone(channel,  serverName + " 471 " + _client.getNick() + " " + channelName + " :Channel client limit set to " + extra + "\n");
    } else if (flag == "-l") {
        channel.setClientLimit(-1);
        sendToEveryone(channel,  serverName + " 471 " + _client.getNick() + " " + channelName + " :Channel has no client limit\n");
    }
}