#include "../../include/Requests.hpp"

void Requests::MODE(Channel *channel, const std::string &flag, const std::string &extra) {
    if (flag == "+i") {
        channel->setInviteOnly(true);
        sendToEveryone(channel, "Channel was set to invite only\n");
    } else if (flag == "-i") {
        channel->setInviteOnly(false);
        sendToEveryone(channel, "Channel is not invite only\n");
    } else if (flag == "+t") {
        channel->setTopicSettableByOp(true);
        sendToEveryone(channel, "Only operator can set channel topic\n");
    } else if (flag == "-t") {
        channel->setTopicSettableByOp(false);
        sendToEveryone(channel, "Everyone can set topic\n");
    } else if (flag == "+k") {
        channel->setKey(extra);
        sendToEveryone(channel, "Channel has a password now\n");
    } else if (flag == "-k") {
        channel->setKey("");
        sendToEveryone(channel, "Channel password was removed\n");
    } else if (flag == "+o") {
        if (_server->getUser(extra) != -1)
            channel->addOperator(extra);
        else
            sendSystemMessage(this->_fd, "No such user " + extra);
        sendToEveryone(channel, extra + " is operator now\n");
    } else if (flag == "-o") {
        if (_server->getUser(extra) != -1)
            channel->removeOperator(extra);
        else
            sendSystemMessage(this->_fd, "No such user " + extra);
        sendToEveryone(channel, extra + " is not operator\n");
    } else if (flag == "+l") {
        channel->setClientLimit(stringToInt(extra));
        sendToEveryone(channel, "Channel has limit of " + extra + " clients\n");
    } else if (flag == "-l") {
        channel->setClientLimit(-1);
        sendToEveryone(channel, "Channel has no limit\n");
    }
}