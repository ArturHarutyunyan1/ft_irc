#include "../../include/Requests.hpp"

std::string Requests::NICK(const std::string &nickname) {
    if (_client.isPasswordSet()) {
        int existingFd = _server.getUser(nickname);

        if (nickname.empty()) {
            return (serverName + " 431 :No nickname given\n");
        }
        if (existingFd != -1 && existingFd != this->_fd) {
            return (serverName + " 433 " + nickname + " :Nickname is already in use\n");
        }
        if (nickname.length() > 32) {
            return (serverName + " 432 " + nickname + " :Erroneous nickname\n");
        }
        if (_client.isNickSet()) {
            _server.removeUser(_client.getNick());
        }
        std::set<std::string> userChannels = _client.getChannels();
        if (!userChannels.empty()) {
            for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); ++it) {
                Channel* channel = _server.getChannel(*it);
                if (channel) {
                    channel->changeClientNickname(_client.getNick(), nickname);
                }
            }
        }
        _server.addUser(nickname, this->_fd);
        _client.setNick(nickname);
        return (":" + nickname + "!user@" + _client.getIP() + " NICK :" + nickname + "\n");

    } else {
        return (serverName + " 462 :You must register first\n");
    }
}
