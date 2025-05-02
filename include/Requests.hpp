#ifndef REQUESTS_HPP
#define REQUESTS_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include "Client.hpp"
#include "Server.hpp"
#include "Utils.hpp"

extern std::string green;
extern std::string red;
extern std::string blue;
extern std::string yellow;
extern std::string reset;
extern std::string serverName;

class Client;
class Server;

class Requests {
public:
	Requests(char *msg, struct pollfd *fds, int fd, std::string _password, Server& _server, Client& _client);
	~Requests();
	
	void handleRequest();
	std::string PASS(std::string const& msg);
	std::string NICK(const std::string &nickname);
	std::string JOIN(const std::string &channel, const std::string &key);
	void PRIVMSG(const std::string &receiver, const std::string &message) const;
	void KICK(const std::string &channel, const std::string &nickname);
	void TOPIC(const std::string &channel, const std::string &topic);
	void INVITE(const std::string &nickname, const std::string &channel);
	void MODE(Channel& channel, const std::string &flag, const std::string &extra);
	void sendToEveryone(Channel const& channel, const std::string &message) const;
	void sendSystemMessage(int fd, const std::string &message) const;

private:
	std::string _password;
	std::string _username;
	char *_message;
	int _fd;
	struct pollfd *_fds;
	Server& _server;
	Client& _client;

	static std::string const green;
	static std::string const red;
	static std::string const blue;
	static std::string const yellow;
	static std::string const reset;
	static std::string const serverName;

	Requests(Requests const&);
	Requests &operator=(Requests const&);
};

#endif