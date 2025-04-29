#include "../include/Requests.hpp"

#include <cstring>

Requests::Requests(char *msg, struct pollfd *fds, int fd, std::string _password, bool isSet, Server *_server, Client *_client)
    : _password(_password), _message(msg), _fd(fd), _fds(fds), _server(_server), _client(_client)
{}

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
    if (spacePos != std::string::npos)
    {
        command = message.substr(0, spacePos);
        args = message.substr(spacePos + 1);
    }
    else
    {
        command = message;
        args = "";
    }
    if (!_client->isAuthenticated() && command != "HELP" && command != "PASS" && command != "NICK" && command != "USER")
        response = "You must authenticate first\nUse HELP command for additional information\n";
    else
    {
        if (command == "HELP" && args.empty())
        {
            response = helpMessage();
        }
        else if (command == "PASS")
        {
            response = PASS(args);
        }
        else if (command == "NICK")
        {
            response = NICK(args);
        }
        else if (command == "USER")
        {
            std::istringstream iss(args);
            std::string username, value, asterix;

            iss >> username >> value >> asterix;

            std::string realname;
            std::getline(iss >> std::ws, realname);

            if (username.empty() || value != "0" || asterix != "*" || realname.empty() || realname[0] != ':')
                response = "ERROR\nUsage - USER <username> 0 * :<realname>\n";
            else
            {
                if (_client->isNickSet() && _client->isPasswordSet() && !_client->isUserSet())
                {
                    realname = realname.substr(1);
                    _client->setUsername(username, realname);
                    response = "Username was set to " + username + " and real name was set to " + realname + "\n";
                } else
                    response = "You must set password and nickname first\n";
            }
        }
        else if (command == "KICK")
        {
            std::istringstream iss(args);
            std::string channel, user, extra;
            iss >> channel >> user >> extra;
            if (!channel.empty() && !user.empty() && extra.empty())
                KICK(channel, user);
            else
                response = "ERROR\nUsage - KICK <channel> <nick>\n";
        }
        else if (command == "TOPIC")
        {
            std::istringstream iss(args);
            std::string channel, topic;

            iss >> channel;
            std::getline(iss >> std::ws, topic);

            if (!channel.empty() && !topic.empty())
                TOPIC(channel, topic);
            else
                response = "ERROR\nUsage - TOPIC <channel> [<topic>]\n";
        }
        else if (command == "INVITE")
        {
            std::istringstream iss(args);
            std::string channel, user, extra;

            iss >> channel >> user >> extra;
            if (!channel.empty() && !user.empty() && extra.empty())
                INVITE(channel, user);
            else
                response = "ERROR\nUsage - INVITE <nickname> <channel>\n";
        }
        else if (command == "MODE")
        {
            std::istringstream iss(message);
            std::string target, channel, flag, extra;

            iss >> target >> channel >> flag >> extra;

            if (channel.empty() || flag.size() < 2 ||
                (flag[0] != '-' && flag[0] != '+') ||
                (flag[1] != 'i' && flag[1] != 't' && flag[1] != 'k' && flag[1] != 'o' && flag[1] != 'l') ||
                ((flag[1] == 'i' || flag[1] == 't' || (flag[0] == '-' && flag[1] == 'k') || (flag[0] == '-' && flag[1] == 'l')) && !extra.empty()) ||
                (((flag[0] == '+' && flag[1] == 'k') || flag[1] == 'o' || (flag[0] == '+' && flag[1] == 'l')) && extra.empty()))
                response = "ERROR\nUsage MODE <target> <flag> [<extra>]\n";
            else
            {
                Channel *channelPtr = _server->getChannel(channel);

                if (channelPtr)
                {
                    MODE(channelPtr, flag, extra);
                }
                else
                    response = "No such channel " + channel + "\n";
            }
        }
        else if (command == "PRIVMSG")
        {
            std::string target, text;

            size_t colonPos = args.find(':');
            if (colonPos == std::string::npos)
                response = "ERROR\nUsage - PRIVMSG <target> :<message>\n";
            else
            {
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
        }
        else if (command == "JOIN")
        {
            std::istringstream iss(args);
            std::string channel, key;

            iss >> channel >> key;
            if (channel.empty() || (channel[0] != '#'))
                response = "ERROR\nUsage - JOIN <#channel>\n";
            else
                response = JOIN(channel, key);
        }
        else
        {
            response = "Unknown Command\n";
        }
    }
    (void)this->_fds;
    send(this->_fd, response.c_str(), response.size(), 0);
}

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

std::string Requests::NICK(const std::string &nickname) {
    if (!_client->isNickSet() && !_client->isUserSet() && _client->isPasswordSet()) {
        int existingFd = this->_server->getUser(nickname);

        if (existingFd != -1 && existingFd != this->_fd)
            return ("Nickname is already taken!\n");
        if (nickname.length() > 9)
            return ("Maximum 9 characters\n");
        std::string previousNick = this->_server->getNick(this->_fd);
        if (previousNick != "NULL")
            this->_server->removeUser(previousNick, this->_fd);
        this->_server->addUser(nickname, this->_fd);
        this->_server->addFd(this->_fd, nickname);
        this->_client->setNick(nickname);
        return ("Nickname was set to " + nickname + "\n");
    } else
        return ("You must enter password before creating NICK\n");
}

void Requests::PRIVMSG(const std::string &receiver, const std::string &message) const {
    int user = _server->getUser(receiver);
    std::string msg;

    if (user != -1) {
        msg = _server->getNick(this->_fd) + " says: " + message + "\n";
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
                        msg = _server->getNick(this->_fd) + " says: " + message + "\n";
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

std::string Requests::JOIN(const std::string &channelName, const std::string &key) {
    Channel *channel = _server->getChannel(channelName);
    std::string nickname = _server->getNick(this->_fd);

    if (nickname == "NULL")
        return ("ERROR\nYou need to set nickname before joining channels\n");
    if (!channel) {
        Channel *newChannel = new Channel(nickname);
        _server->addChannel(channelName, newChannel);
        return ("Channel " + channelName + " was created\n");
    }
    ChannelClientStatus status = channel->addClient(nickname, key);

    if (status == CHANNEL_CLIENT_ALREADY_IN)
        return ("ERROR: You are already in the channel\n");
    else if (status == CHANNEL_CLIENT_NOT_INVITED)
        return ("ERROR: Channel is invite-only\n");
    else if (status == CHANNEL_NOT_ENOUGH_PLACES)
        return ("ERROR: Channel is full\n");
    else if (status == CHANNEL_INVALID_KEY)
        return ("ERROR: Invalid channel key\n");
    else
    {
        if (channel->getTopic().size() > 0) {
            std::string topic = "TOPIC - " + channel->getTopic() + "\n";
            send(this->_fd, topic.c_str(), topic.size(), 0);
        }
        return ("You have joined channel " + channelName + "\n");
    }
}

void Requests::KICK(const std::string &channelName, const std::string &nickname) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        if (channel->isOperator(_server->getNick(this->_fd))) {
            if (_server->getNick(this->_fd) != nickname) {
                sendToEveryone(channel, _server->getNick(this->_fd) + " kicked " + nickname + " from channel " + channelName + "\n");
                channel->kickClient(nickname);
            } else {
                sendSystemMessage(this->_fd, "You can't kick yourself\n");
            }
        } else
            sendSystemMessage(this->_fd, "You are not operator\n");
    } else
        sendSystemMessage(this->_fd, "No such channel " + channelName + "\n");
}

void Requests::TOPIC(const std::string &channelName, const std::string &topic) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        if (channel->getTopicSettableByOp()) {
            if (channel->isOperator(_server->getNick(this->_fd))) {
                channel->setTopic(topic);
                sendToEveryone(channel, "Channel topic was set to " + topic + "\n");
            } else
                sendSystemMessage(this->_fd, "Only operator can set topic\n");
        } else {
            channel->setTopic(topic);
            sendToEveryone(channel, "Channel topic was set to " + topic + "\n");
        }
    } else
        sendSystemMessage(this->_fd, "No such channel " + channelName + "\n");
}

void Requests::sendToEveryone(Channel *channel, const std::string &message) const {
    std::set<std::string> clients = channel->getClients();

    for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        int clientFD = _server->getUser(*it);
        send(clientFD, message.c_str(), message.size(), 0);
    }
}

void Requests::INVITE(const std::string &channelName, const std::string &nickname) {
    Channel *channel = _server->getChannel(channelName);

    if (channel) {
        int user = _server->getUser(nickname);

        if (user != -1) {
            channel->inviteClient(nickname);
            sendSystemMessage(_server->getUser(nickname), _server->getNick(this->_fd) + " invited you to channel " + channelName + "\n");
        } else {
            sendSystemMessage(this->_fd, "No such user " + nickname + "\n");
        }
    } else {
        sendSystemMessage(this->_fd, "No such channel " + channelName + "\n");
    }
}

void Requests::MODE(Channel *channel, const std::string &flag, const std::string &extra) {
    if (flag == "+i") {
        channel->setInviteOnly(true);
        sendToEveryone(channel, "Channel was set to invite only\n");
    } else if (flag == "-i") {
        channel->setInviteOnly(false);
        sendToEveryone(channel, "Channel is not invite only\n");
    } else if (flag == "+t") {
        channel->setTopicSettableByOp(true);
        sendToEveryone(channel, "Only operator can set channel topic\n");
    } else if (flag == "-t") {
        channel->setTopicSettableByOp(false);
        sendToEveryone(channel, "Everyone can set topic\n");
    } else if (flag == "+k") {
        channel->setKey(extra);
        sendToEveryone(channel, "Channel has a password now\n");
    } else if (flag == "-k") {
        channel->setKey("");
        sendToEveryone(channel, "Channel password was removed\n");
    } else if (flag == "+o") {
        if (_server->getUser(extra) != -1)
            channel->addOperator(extra);
        else
            sendSystemMessage(this->_fd, "No such user " + extra);
        sendToEveryone(channel, extra + " is operator now\n");
    } else if (flag == "-o") {
        if (_server->getUser(extra) != -1)
            channel->removeOperator(extra);
        else
            sendSystemMessage(this->_fd, "No such user " + extra);
        sendToEveryone(channel, extra + " is not operator\n");
    } else if (flag == "+l") {
        channel->setClientLimit(stringToInt(extra));
        sendToEveryone(channel, "Channel has limit of " + extra + " clients\n");
    } else if (flag == "-l") {
        channel->setClientLimit(-1);
        sendToEveryone(channel, "Channel has no limit\n");
    }
}

void Requests::sendSystemMessage(int fd, const std::string &message) const {
    send(fd, message.c_str(), message.size(), 0);
}
