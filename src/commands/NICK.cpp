#include "../../include/Requests.hpp"

std::string Requests::NICK(const std::string &nickname) {
    if (_client->isPasswordSet()) {
        int existingFd = this->_server->getUser(nickname);

        if (existingFd != -1 && existingFd != this->_fd)
            return ("Nickname is already taken!\n");
        if (nickname.length() > 9)
            return ("Maximum 9 characters\n");
        std::string previousNick = this->_server->getNick(this->_fd);
        if (previousNick != "NULL")
            this->_server->removeUser(previousNick, this->_fd);
        std::set<std::string> userChannels = _client->getChannels();
        if (userChannels.size() > 0) {
            for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); ++it) {
                Channel *channel = _server->getChannel(*it);
                channel->changeClientNickname(previousNick, nickname);
            }
        }
        this->_server->addUser(nickname, this->_fd);
        this->_server->addFd(this->_fd, nickname);
        this->_client->setNick(nickname);
        return ("Nickname was set to " + nickname + "\n");
    } else
        return ("You must enter password before creating NICK\n");
}