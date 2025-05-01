#include "../../include/Requests.hpp"

std::string Requests::JOIN(const std::string &channelName, const std::string &key) {
    Channel *channel = _server->getChannel(channelName);
    std::string nickname = _server->getNick(this->_fd);

    if (nickname == "NULL")
        return (red + serverName + " 451 :You have not registered" + reset + "\n");
    if (!channel) {
        Channel *newChannel = new Channel(nickname);
        _server->addChannel(channelName, newChannel);
        _client->addChannel(channelName);
        channel = newChannel;
        sendToEveryone(channel, green + ":" + _client->getNick() + "!" + _client->getUsername() + "@" + _client->getIP() + " JOIN " + channelName + reset + "\n");
        return ("");
    }
    ChannelClientStatus status = channel->addClient(nickname, key);

    if (status == CHANNEL_CLIENT_ALREADY_IN) {
        return(red +
            ":" + serverName + " 443 " + nickname + " " + channelName + " :You're already on that channel" + reset + "\n"
        );
    }
    else if (status == CHANNEL_CLIENT_NOT_INVITED) {
        return(red +
            ":" + serverName + " 473 " + nickname + " " + channelName + " :Cannot join channel" + reset + "\n"
        );
    }
    else if (status == CHANNEL_NOT_ENOUGH_PLACES) {
        return(red +
            ":" + serverName + " 471 " + nickname + " " + channelName + " :Cannot join channel" + reset + "\n"
        );
    }
    else if (status == CHANNEL_INVALID_KEY) {
        return(red +
            ":" + serverName + " 475 " + nickname + " " + channelName + " :Cannot join channel" + reset + "\n"
        );
    }
    else
    {
        sendToEveryone(channel, green + ":" + _client->getNick() + "!" + _client->getUsername() + "@" + _client->getIP() + " JOIN " + channelName + reset + "\n");
        if (channel->getTopic().size() > 0)
            sendSystemMessage(this->_fd, yellow + serverName + ": 332 " + nickname + " " + channelName + ":" + channel->getTopic() + reset + "\n");
        else
            sendSystemMessage(this->_fd, yellow + serverName + ": 332 " + nickname + " " + channelName + ":No topic" + reset + "\n");        
        return ("");
    }
}
