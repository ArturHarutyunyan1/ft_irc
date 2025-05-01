#include "../../include/Requests.hpp"

std::string Requests::JOIN(const std::string &channelName, const std::string &key) {
    Channel *channel = _server->getChannel(channelName);
    std::string nickname = _server->getNick(this->_fd);

    if (nickname == "NULL")
        return ("ERROR\nYou need to set nickname before joining channels\n");
    if (!channel) {
        Channel *newChannel = new Channel(nickname);
        _server->addChannel(channelName, newChannel);
        _client->addChannel(channelName);
        return ("Channel " + channelName + " was created\n");
    }
    ChannelClientStatus status = channel->addClient(nickname, key);

    if (status == CHANNEL_CLIENT_ALREADY_IN)
        return ("ERROR: You are already in the channel\n");
    else if (status == CHANNEL_CLIENT_NOT_INVITED)
        return ("ERROR: Channel is invite-only\n");
    else if (status == CHANNEL_NOT_ENOUGH_PLACES)
        return ("ERROR: Channel is full\n");
    else if (status == CHANNEL_INVALID_KEY)
        return ("ERROR: Invalid channel key\n");
    else
    {
        if (channel->getTopic().size() > 0) {
            std::string topic = "TOPIC - " + channel->getTopic() + "\n";
            send(this->_fd, topic.c_str(), topic.size(), 0);
        }
        return ("You have joined channel " + channelName + "\n");
    }
}