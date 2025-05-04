#include "../../include/Requests.hpp"

std::string Requests::NICK(const std::string &nickname) {
    if (!_client.isPasswordSet()) {
        return (serverName + " 462 :You must register first\n");
    }

    if (nickname.empty()) {
        return (serverName + " 431 :No nickname given\n");
    }

    if (nickname.length() > 32) {
        return (serverName + " 432 " + nickname + " :Erroneous nickname\n");
    }

    int existingFd = _server.getUser(nickname);
    if (existingFd != -1 && existingFd != this->_fd) {
        return (serverName + " 433 " + nickname + " :Nickname is already in use\n");
    }

    std::string oldNick = _client.getNick();
    if (_client.isNickSet()) {
        _server.removeUser(oldNick);
    }

    std::set<std::string> userChannels = _client.getChannels();
    for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); ++it) {
        Channel* channel = _server.getChannel(*it);
        if (channel) {
            channel->changeClientNickname(oldNick, nickname);
        }
    }
    _server.addUser(nickname, this->_fd);
    _client.setNick(nickname);
    
    std::string replyNick = oldNick.empty() ? nickname : oldNick;
    return (":" + replyNick + "!user@" + _client.getIP() + " NICK :" + nickname + "\n");
}