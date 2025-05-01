#include "../../include/Requests.hpp"

void Requests::PRIVMSG(const std::string &receiver, const std::string &message) const {
    int user = _server->getUser(receiver);
    std::string msg;

    if (user != -1) {
        msg = ":" + _server->getNick(this->_fd) + "!" + _client->getUsername() + "@" + _client->getIP() + " PRIVMSG " + receiver + " :" + message + "\n"; 
        send(user, msg.c_str(), msg.size(), 0);
    } else if (receiver[0] == '#') {
        Channel *channel = _server->getChannel(receiver);
        if (channel) {
            if (!channel->isClient(_server->getNick(this->_fd))) {
                msg = "You are not in " + receiver + " channel\n";
                send(this->_fd, msg.c_str(), msg.size(), 0);
            } else {
                std::set<std::string> clients = channel->getClients();

                for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it) {
                    int clientFD = _server->getUser(*it);

                    if (clientFD != this->_fd) {
                        msg = ":" + _server->getNick(this->_fd) + "!" + _client->getUsername() + "@" + _client->getIP() + " PRIVMSG " + receiver + " :" + message + "\n"; 
                        send(clientFD, msg.c_str(), msg.size(), 0);
                    }
                }
            }
        } else {
            msg = "No such channel " + receiver + "\n";
            send(this->_fd, msg.c_str(), msg.size(), 0);
        }
    } else {
        msg = "No such user " + receiver + "\n";
        send(this->_fd, msg.c_str(), msg.size(), 0);
    }
}