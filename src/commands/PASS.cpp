#include "../../include/Requests.hpp"


std::string Requests::PASS(std::string const& msg)
{
    if (msg.empty()) {
        return (serverName + " 461 :Not enough parameters\n");
    }
    
    if (_client.isAuthenticated()) {
        return (serverName + " 462 :You are already registered\n");
    }
    
    if (msg == _server.getPassword() && !_client.isPasswordSet()) {
        _client.setPassword(msg);
        return (serverName + " 920 :Password accepted\n");
    } else {
        return (serverName + " 464 :Invalid password\n");
    }
}
