#include "../../include/Requests.hpp"

std::string Requests::PASS(std::string msg)
{
    if (msg == _server->getPassword() && !_client->isAuthenticated() && !_client->isPasswordSet()) {
        _client->setPassword(msg);
        return ("Password was set successfully\n");
    } else if (_client->isAuthenticated())
        return ("You are already authenticated\n");
    else
        return ("Invalid password\n");
}