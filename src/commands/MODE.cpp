#include "../../include/Requests.hpp"

void Requests::MODE(Channel *channel, const std::string &flag, const std::string &extra) {
    if (!channel->isOperator(_client->getNick()))
        sendSystemMessage(this->_fd, red + serverName + " :You must be operator" + reset + "\n");
    else {
        if (flag == "+i") {
            channel->setInviteOnly(true);
            sendToEveryone(channel, yellow + serverName + " :Channel was set to invite only " + reset + "\n");
        } else if (flag == "-i") {
            channel->setInviteOnly(false);
            sendToEveryone(channel, yellow + serverName + " :Channel is not invite only " + reset + "\n");
        } else if (flag == "+t") {
            channel->setTopicSettableByOp(true);
            sendToEveryone(channel, yellow + serverName + " :Only operator can set topic " + reset + "\n");
        } else if (flag == "-t") {
            channel->setTopicSettableByOp(false);
            sendToEveryone(channel, yellow + serverName + " :All users can set topic " + reset + "\n");
        } else if (flag == "+k") {
            channel->setKey(extra);
            sendToEveryone(channel, yellow + serverName + " :Channel has password now " + reset + "\n");
        } else if (flag == "-k") {
            channel->setKey("");
            sendToEveryone(channel, yellow + serverName + " :Channel password was removed " + reset + "\n");
        } else if (flag == "+o") {
            if (_server->getUser(extra) != -1)
                channel->addOperator(extra);
            else
                sendSystemMessage(this->_fd, red + serverName + " :No such user " + extra + reset + "\n");
            sendToEveryone(channel, yellow + serverName + " : " + extra + " is operator now" + reset + "\n");
        } else if (flag == "-o") {
            if (_server->getUser(extra) != -1)
                channel->removeOperator(extra);
            else
                sendSystemMessage(this->_fd, red + serverName + " :No such user " + extra + reset + "\n");
            sendToEveryone(channel, yellow + serverName + " : " + extra + " is not operator" + reset + "\n");
        } else if (flag == "+l") {
            channel->setClientLimit(stringToInt(extra));
            sendToEveryone(channel, yellow + serverName + " :Channel has limit of " + extra + " clients" + reset + "\n");
        } else if (flag == "-l") {
            channel->setClientLimit(-1);
            sendToEveryone(channel, yellow + serverName + " :Channel has no limit" + reset + "\n");
        }
    }
}
