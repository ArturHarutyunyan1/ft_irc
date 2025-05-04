#include "../include/Requests.hpp"

std::string const Requests::serverName = ":super_mega_cool_irc_server";

Requests::Requests(char *msg, struct pollfd *fds, int fd, std::string _password, Server &_server, Client &_client)
	: _password(_password), _message(msg), _fd(fd), _fds(fds), _server(_server), _client(_client)
{
}

Requests::Requests(const Requests &other)
	: _password(other._password), _username(other._username),
	  _fd(other._fd), _fds(other._fds), _server(other._server),
	  _client(other._client)
{
	if (other._message != NULL)
	{
		_message = new char[strlen(other._message) + 1];
		strcpy(_message, other._message);
	}
	else
	{
		_message = NULL;
	}
}

Requests &Requests::operator=(const Requests &)
{
	return *this;
}

Requests::~Requests()
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
	if (!_client.isAuthenticated() && command != "HELP" && command != "PASS" && command != "NICK" && command != "USER")
		response = ":" + serverName + " 451 " + " :You must authenticate first. Use HELP for more info" + "\n";
	else
	{
		if (command == "HELP" && args.empty())
			response = helpMessage();
		else if (command == "PASS")
			response = PASS(args);
		else if (command == "NICK")
			response = NICK(args);
		else if (command == "USER")
		{
			std::istringstream iss(args);
			std::string username, value, asterix;

			iss >> username >> value >> asterix;

			std::string realname;
			std::getline(iss >> std::ws, realname);

			if (username.empty() || value.empty() || asterix.empty() || realname.empty() || realname[0] != ':')
				response = "ERROR\nUsage: USER <username> 0 * :<realname>\n";
			else
			{
				if (_client.isNickSet() && _client.isPasswordSet() && !_client.isUserSet())
				{
					realname = realname.substr(1);
					_client.setUsername(username, realname);
					response = serverName + " 001 " + _client.getNick() + " :Welcome to Welcome to the Internet Relay Network " + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP() + "\n";
				}
				else
					response = serverName + " 462 :You must enter password and crate nick before creating USER" + "\n";
			}
		}
		else if (command == "KICK")
		{
			std::istringstream iss(args);
			std::string channel, user;
			iss >> channel >> user;

			std::string reason;
			std::getline(iss, reason);
			if (!reason.empty() && reason[0] == ' ')
				reason.erase(0, 1); // Remove leading space

			if (!channel.empty() && !user.empty())
			{
				KICK(channel, user, reason);
			}
			else
			{
				response = serverName + " 461 " + _client.getNick() + " :Usage - KICK <channel> <nick> [reason]" + "\n";
			}
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
				response = serverName + " 461 " + _client.getNick() + " :Usage - TOPIC <channel> [<topic>]" + "\n";
		}
		else if (command == "INVITE")
		{
			std::istringstream iss(args);
			std::string channel, user, extra;

			iss >> channel >> user >> extra;
			if (!channel.empty() && !user.empty() && extra.empty())
				INVITE(channel, user);
			else
				response = serverName + " 461 " + _client.getNick() + " :Usage - INVITE <nickname> <channel>" + "\n";
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
				response = serverName + " 461 " + _client.getNick() + " :Usage - MODE <target> <flag> [<extra>]\n";
			else
			{
				Channel *channelPtr = _server.getChannel(channel);

				if (channelPtr)
				{
					MODE(*channelPtr, target, flag, extra);
				}
				else
					response = serverName + ": 412 " + _client.getNick() + " :No such channel" + "\n";
			}
		}
		else if (command == "PRIVMSG")
		{
			std::string target, text;

			size_t colonPos = args.find(':');
			if (colonPos == std::string::npos)
				response = serverName + " 461 " + _client.getNick() + " :Usage - PRIVMSG <target> :<message>\n";
			else
			{
				target = args.substr(0, colonPos);
				text = args.substr(colonPos + 1);

				size_t end = target.find_last_not_of(" ");
				if (end != std::string::npos)
					target = target.substr(0, end + 1);
				if (target.empty() || text.empty())
					response = serverName + " 461 " + _client.getNick() + " :Usage - PRIVMSG <target> :<message>" + "\n";
				else
				{
					PRIVMSG(target, text);
				}
			}
		}
		else if (command == "JOIN")
		{
			std::istringstream iss(args);
			std::string channel, key;

			iss >> channel >> key;
			if (channel.empty() || (channel[0] != '#'))
				response = serverName + " 461 " + _client.getNick() + " :Usage - JOIN <#channel>\n";
			else
				JOIN(channel, key);
		}
		else if (command == "PART")
		{
			std::istringstream iss(args);
			std::string channelName;
			iss >> channelName;

			std::string reason;
			std::getline(iss >> std::ws, reason);
			if (channelName.empty())
			{
				response = serverName + " 461 " + _client.getNick() + " :Usage - PART <#channel> [:<reason>]\r\n";
			}
			else
			{
				Channel *channel = _server.getChannel(channelName);

				if (!channel)
				{
					response = serverName + " 403 " + _client.getNick() + " " + channelName + " :No such channel\r\n";
				}
				else if (!channel->isClient(_client.getNick()))
				{
					response = serverName + " 442 " + _client.getNick() + " " + channelName + " :You're not on that channel\r\n";
				}
				else
				{
					if (!reason.empty() && reason[0] == ':')
						reason = reason.substr(1);
					std::string msg = ":" + _client.getNick() + "!" + _client.getUsername() + "@" + _client.getIP() + " PART " + channelName;
					if (!reason.empty())
					{
						msg += " :" + reason;
					}
					msg += "\r\n";
					sendToEveryone(*channel, msg);
					channel->kickClient(_client.getNick());
					_client.removeChannel(channelName);

					if (channel->getClients().size() == 0)
						_server.removeChannel(channelName);
				}
			}
		}
		else
			response = serverName + " :No such command" + "\n";
	}
	(void)this->_fds;
	send(this->_fd, response.c_str(), response.size(), 0);
}

void Requests::sendToEveryone(Channel const &channel, const std::string &message) const
{
	std::set<std::string> clients = channel.getClients();

	for (std::set<std::string>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		int clientFD = _server.getUser(*it);
		send(clientFD, message.c_str(), message.size(), 0);
	}
}

void Requests::sendSystemMessage(int fd, const std::string &message) const
{
	send(fd, message.c_str(), message.size(), 0);
}
