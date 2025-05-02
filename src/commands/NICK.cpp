#include "../../include/Requests.hpp"

std::string Requests::NICK(const std::string &nickname) {
	if (_client.isPasswordSet()) {
		int existingFd = _server.getUser(nickname);
		if (nickname.empty())
			return (red + serverName + " 431 :No nickname given" + reset + "\n");
		if (existingFd != -1 && existingFd != this->_fd)
			return (red + serverName + " 433 " + nickname + " :Nickname is already in use" + reset + "\n");
		if (nickname.length() > 9)
			return (red + serverName + " 432 " + nickname + " :Erroneous nickname" + reset + "\n");
		if (_client.isNickSet())
			_server.removeUser(_client.getNick());
		std::set<std::string> userChannels = _client.getChannels();
		if (userChannels.size() > 0) {
			for (std::set<std::string>::iterator it = userChannels.begin(); it != userChannels.end(); ++it) {
				Channel *channel = _server.getChannel(*it);
				channel->changeClientNickname(_client.getNick(), nickname);
			}
		}
		_server.addUser(nickname, this->_fd);
		_client.setNick(nickname);
		return (green + ":" + nickname + "!user@" + _client.getIP() + " NICK :" + nickname + reset + "\n");
	} else
		return (red + serverName + " 462 :You must enter password before crating NICK" + reset + "\n");
}