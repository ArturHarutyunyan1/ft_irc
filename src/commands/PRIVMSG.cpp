#include "../../include/Requests.hpp"

void Requests::PRIVMSG(const std::string &receiver, const std::string &message) const {
	int user = _server.getUser(receiver);
	std::string prefix = ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP();

	if (message.empty()) {
		sendSystemMessage(this->_fd, serverName + " 412 " + _client.getNick() + " :No text to send\n");
		return;
	}

	if (user != -1) {
		sendSystemMessage(user, prefix + " PRIVMSG " + receiver + " :" + message + "\n");
	}
	else if (!receiver.empty() && receiver[0] == '#') {
		Channel *channel = _server.getChannel(receiver);
		if (!channel) {
			sendSystemMessage(this->_fd, serverName + " 401 " + _client.getNick() + " " + receiver + " :No such channel\n");
			return;
		}
		if (!channel->isClient(_client.getNick())) {
			sendSystemMessage(this->_fd, serverName + " 442 " + _client.getNick() + " " + receiver + " :You're not on that channel\n");
			return;
		}
		std::set<std::string> clients = channel->getClients();
		for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it) {
			int clientFD = _server.getUser(*it);
			if (clientFD != this->_fd) {
				sendSystemMessage(clientFD, prefix + " PRIVMSG " + receiver + " :" + message + "\n");
			}
		}
	}
	else {
		sendSystemMessage(this->_fd, serverName + " 401 " + _client.getNick() + " " + receiver + " :No such nick\n");
	}
}
