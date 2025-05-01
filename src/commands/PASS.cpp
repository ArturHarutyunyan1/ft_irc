#include "../../include/Requests.hpp"


std::string Requests::PASS(std::string msg)
{
    if (msg == _server->getPassword() && !_client->isAuthenticated() && !_client->isPasswordSet()) {
        _client->setPassword(msg);
        return (green + serverName + " 920 :Password accepted" + reset + "\n");
    } else if (_client->isAuthenticated()) {
        return (red + serverName + " 462 :You may not re register" + reset + "\n");
    } else if (msg.empty()) {
        return(red + serverName + " 461 :Not enough parameters" + reset + "\n");
    } else {
        return (red + serverName + " 464 :Invalid password" + reset + "\n");
    }
}
