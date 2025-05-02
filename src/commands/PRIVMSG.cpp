#include "../../include/Requests.hpp"

void Requests::PRIVMSG(const std::string &receiver, const std::string &message) const {
	int user = _server.getUser(receiver);
	std::string msg;

	if (user != -1) {
		sendSystemMessage(user, blue +  _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP() + " PRIVMSG " + receiver + " :" + message + reset + "\n");
	} else if (receiver[0] == '#') {
		Channel *channel = _server.getChannel(receiver);
		if (channel) {
			if (!channel->isClient(_client.getNick())) {
				sendSystemMessage(this->_fd, red + serverName + ": 412 " + _client.getNick() + " :No such nickname" + reset + "\n");
			} else {
				std::set<std::string> clients = channel->getClients();

				for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it) {
					int clientFD = _server.getUser(*it);

					if (clientFD != this->_fd)
						sendSystemMessage(clientFD, blue + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP() + " PRIVMSG " + receiver + " :" + message + reset + "\n");
				}
			}
		} else
		sendSystemMessage(this->_fd, red + serverName + ": 412 " + _client.getNick() + " :No such channel" + reset + "\n");
	} else
		sendSystemMessage(this->_fd, red + serverName + ": 412 " + _client.getNick() + " :No such nickname" + reset + "\n");
}