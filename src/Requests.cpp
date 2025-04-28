#include "../include/Requests.hpp"

#include <cstring>

Requests::Requests(char *msg, struct pollfd *fds, int fd, std::string _password, bool isSet, Server *_server) : _password(_password), _message(msg), _fd(fd), _fds(fds), _isSet(isSet), _server(_server)
{
}

void Requests::handleRequest()
{
    std::string message(_message);
    std::string response;
    std::string command;
    std::string args;

    size_t end = message.size();
    while (end > 0 && (message[end - 1] == ' ' || message[end - 1] == '\n' || message[end - 1] == '\r'))
        --end;
    message = message.substr(0, end);

    size_t spacePos = message.find(' ');
    if (spacePos != std::string::npos) {
        command = message.substr(0, spacePos);
        args = message.substr(spacePos + 1);
    } else {
        command = message;
        args = "";
    }

    if (command == "HELP" && args.empty()) {
        response = helpMessage();
    } else if (command == "PASS") {
        response = PASS(args);
    } else if (command == "NICK") {
        response = NICK(args);
    } else if (command == "USER") {
        std::istringstream iss(args);
        std::string username, value, asterix, realname, extra;

        iss >> username >> value >> asterix >> realname >> extra;

        if (username.empty() || value.empty() || value != "0" || asterix.empty() || asterix != "*" || realname.empty() || realname[0] != ':' || !extra.empty())
            response = "ERROR\nUsage - USER <username> 0 * :<realname>\n";
        else 
            response = "Parsed USER command for username=" + username + " and realname=" + realname + "\n";
    } else if (command == "KICK") {
        std::istringstream iss(args);
        std::string channel, user, extra;
        iss >> channel >> user >> extra;
        if (!channel.empty() && !user.empty() && extra.empty())
            response = "KICK command parsed for channel=" + channel + " and user=" + user + "\n";
        else
            response = "ERROR\nUsage - KICK <channel> <nick>\n";
    } else if (command == "TOPIC") {
        std::istringstream iss(args);
        std::string channel, topic, extra;

        iss >> channel >> topic >> extra;
        if (!channel.empty() && !topic.empty() && extra.empty())
            response = "TOPIC command parsed for chanel=" + channel + " and topic=" + topic + "\n";
        else
            response = "ERROR\nUsage - TOPIC <channel> [<topic>]\n";
    } else if (command == "INVITE") {
        std::istringstream iss(args);
        std::string channel, user, extra;

        iss >> channel >> user >> extra;
        if (!channel.empty() && !user.empty() && extra.empty())
            response = "INVITE command parsed for chanel=" + channel + " and user=" + user + "\n";
        else
            response = "ERROR\nUsage - INVITE <nickname> <channel>\n";
    } else if (command == "MODE") {
        std::istringstream iss(message);
        std::string target, flag, extra;

        iss >> target >> flag >> extra;
        if (target.empty() || flag.empty() || !extra.empty() ||
            flag.length() != 2 ||
            (flag[0] != '+' && flag[0] != '-') ||
            (flag[1] != 'i' && flag[1] != 't' && flag[1] != 'k' && flag[1] != 'o' && flag[1] != 'l'))
                response = "ERROR\nUsage MODE <target> <flag>\n";
        else {
            // HANDLE MODE COMMAND
        }
    } else if (command == "PRIVMSG") {
        std::string target, text;

        size_t colonPos = args.find(':');
        if (colonPos == std::string::npos)
            response = "ERROR\nUsage - PRIVMSG <target> :<message>\n";
        else {
            target = args.substr(0, colonPos);
            text = args.substr(colonPos + 1);

            size_t end = target.find_last_not_of(" ");
            if (end != std::string::npos)
                target = target.substr(0, end + 1);
            if (target.empty() || text.empty())
                response = "ERROR\nUsage - PRIVMSG <target> :<message>\n";
            else
                PRIVMSG(target, text);
        }
    } else if (command == "JOIN") {
        std::istringstream iss(args);
        std::string channel, key;

        iss >> channel >> key;
        if (channel.empty() || (channel[0] != '#'))
            response = "ERROR\nUsage - JOIN <#channel>\n";
        else
            // response = JOIN(channel);
            response = "JOIN\n";
    } else {
        response = "Unknown Command\n";
    }
    (void)this->_fds;
    send(this->_fd, response.c_str(), response.size(), 0);
}

std::string Requests::PASS(std::string msg)
{
    if (msg == _password && !_isSet)
    {
        _isSet = true;
        return ("Password was set successfully!\n");
    }
    else
        return ("Invalid password!\n");
}

std::string Requests::NICK(const std::string &nickname) {
    int existingFd = this->_server->getUser(nickname);

    if (existingFd != -1 && existingFd != this->_fd)
        return ("Nickname is already taken!\n");
    if (nickname.length() > 9)
        return ("Maximum 9 characters\n");
    std::string previousNick = this->_server->getNick(this->_fd);
    if (previousNick != "NULL")
        this->_server->removeUser(previousNick, this->_fd);
    this->_server->addUser(nickname, this->_fd);
    return ("Nickname was set to " + nickname + "\n");
}

// Send message to specific user
void Requests::PRIVMSG(const std::string &receiver, const std::string &message) const {
    if (receiver == "bot")
    {
        std::string response = this->_server->sendRequestToBot(message, this->_server->getBot().socketFd);
        response = "bot: " + response + "\n";
        send(this->_fd, response.c_str(), response.size(), 0);
        return ;
    }
    int user = this->_server->getUser(receiver);

    if (user != -1) {
        std::string msg = this->_server->getNick(this->_fd) + " says: " + message + "\n";
        send(user, msg.c_str(), msg.size(), 0);
    }
}

// std::string Requests::JOIN(const std::string &channel) {
//     std::string nickname = this->_server->getNick(this->_fd);

//     if (nickname.empty())
//         return ("JOIN failed: nickname not set\n");
//     bool success = this->_server->joinChannel(channel, nickname);

//     if (!success)
//         return ("JOIN failed: Client client could not be added to channel " + channel + "\n");
//     else
//         return (nickname + " joined channel " + channel + '\n');
// }