#include "../include/Server.hpp"
#include <cerrno>
#include <sys/poll.h>
#include <sys/socket.h>
#include <map>

Server::Server(int port, std::string password) : _port(port), _password(password)
{
	this->_client_fds = new struct pollfd[MAX_CONNECTIONS];
}

Server::~Server()
{
	std::cout << "Destructor" << std::endl;

	for (int i = 0; i < MAX_CONNECTIONS; ++i)
	{
		if (_client_fds[i].fd != -1)
		{
			close(_client_fds[i].fd);
			_client_fds[i].fd = -1;
		}
	}

	delete[] _client_fds;

	std::cout << "Server resources cleaned up." << std::endl;
}

Server::Server(const Server &other)
	: _port(other._port), _host(other._host), _password(other._password)
{
	_client_fds = other._client_fds;

	_channels = other._channels;
	_usernameToFd = other._usernameToFd;
	_clients = other._clients;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_port = other._port;
		_host = other._host;
		_password = other._password;
		_client_fds = other._client_fds;
		_channels = other._channels;
		_usernameToFd = other._usernameToFd;
		_clients = other._clients;
	}
	return *this;
}

void Server::cleanup()
{
	std::cout << "Cleaning up all sockets..." << std::endl;

	for (int i = 0; i < MAX_CONNECTIONS; ++i)
	{
		if (_client_fds[i].fd != -1)
		{
			close(_client_fds[i].fd);
			_client_fds[i].fd = -1;
		}
	}
	int serverSocket = _client_fds[0].fd;
	if (serverSocket != -1)
	{
		close(serverSocket);
		_client_fds[0].fd = -1;
	}
	std::cout << "All resources have been cleaned up." << std::endl;
}

int Server::getPort() const
{
	return (this->_port);
}

std::string const &Server::getPassword() const
{
	return (this->_password);
}

void Server::newClient(int fd)
{
	sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int newFD = accept(fd, (struct sockaddr *)&clientAddr, &clientLen);
	if (newFD == -1)
	{
		perror("Accept error");
	}
	else
	{
		fcntl(newFD, F_SETFL, O_NONBLOCK);

		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
		std::string ipAddress(ipStr); // <- this is the IP

		int idx = this->findFreeFdSlot();

		if (idx != -1)
		{
			this->_client_fds[idx].fd = newFD;
			this->_client_fds[idx].events = POLLIN;
			std::cout << newFD << std::endl;
			std::cout << "New client connected ✨ IP: " << ipAddress << std::endl;

			std::string welcome = welcomeMessage();
			if (send(newFD, welcome.c_str(), welcome.size(), 0) == -1)
				perror("Send error");

			_clients[newFD] = Client(ipAddress);
		}
		else
		{
			std::cerr << "No available slots for new client" << std::endl;
			close(newFD);
		}
	}
}

int Server::findFreeFdSlot()
{
	if (this->_client_fds != NULL)
	{
		for (int i = 1; i < MAX_CONNECTIONS; i++)
		{
			if (this->_client_fds[i].fd == -1)
				return i;
		}
	}

	return -1;
}

void Server::handleRequest(int i)
{
	char buffer[1024] = {0};
	int client_fd = this->_client_fds[i].fd;
	ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes <= 0)
	{
		if (bytes == 0)
		{
			std::cout << "Client disconnected 🚪" << std::endl;
		}
		else
		{
			std::cerr << "recv() error" << std::endl;
		}
		close(client_fd);
		this->_client_fds[i].fd = -1;
		this->_client_fds[i].events = 0;
		this->_clients.erase(client_fd);
		_clientBuffers.erase(client_fd);
		return;
	}

	buffer[bytes] = '\0';
	std::string &raw = _clientBuffers[client_fd];
	raw.append(buffer);

	size_t pos;
	while ((pos = raw.find("\n")) != std::string::npos)
	{
		std::string line = raw.substr(0, pos);
		raw.erase(0, pos + 1);

		if (!line.empty() && line[line.size() - 1] == '\r')
		{
			line.erase(line.size() - 1);
		}

		if (!line.empty())
		{
			Requests req(const_cast<char *>(line.c_str()), this->_client_fds, client_fd, getPassword(), *this, _clients[client_fd]);
			req.handleRequest();
		}
	}
}

void Server::addUser(const std::string &nickname, int fd)
{
	_usernameToFd[nickname] = fd;
}

void Server::removeUser(const std::string &nickname)
{
	_usernameToFd.erase(nickname);
}

int Server::getUser(const std::string &nickname) const
{
	std::map<std::string, int>::const_iterator it = _usernameToFd.find(nickname);

	if (it != _usernameToFd.end())
		return (it->second);
	return (-1);
}

void Server::removeChannel(const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);

	if (it != _channels.end())
		_channels.erase(it);
}

Channel *Server::getChannel(const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);

	if (it != _channels.end())
		return &(it->second);
	return (NULL);
}

void Server::addChannel(const std::string &channelName, Channel &channel)
{
	_channels[channelName] = channel;
}

void Server::start()
{
	int serverSocket;
	int opt = 1;

	serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (serverSocket == -1)
	{
		perror("Socket creation failed");
		return;
	}

	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(getPort());
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("Bind failed");
		close(serverSocket);
		return;
	}

	if (listen(serverSocket, MAX_CONNECTIONS) == -1)
	{
		perror("Listen failed");
		close(serverSocket);
		return;
	}

	for (int i = 1; i < MAX_CONNECTIONS; i++)
	{
		this->_client_fds[i].fd = -1;
		this->_client_fds[i].events = 0;
	}
	this->_client_fds[0].fd = serverSocket;
	this->_client_fds[0].events = POLLIN;

	while (1)
	{
		int poll_count = poll(this->_client_fds, MAX_CONNECTIONS, -1);
		if (poll_count == -1)
		{
			perror("Poll error");
			exit(1);
		}

		for (int i = 0; i < MAX_CONNECTIONS; i++)
		{
			if (this->_client_fds[i].revents & (POLLIN | POLLOUT))
			{
				if (this->_client_fds[i].fd == serverSocket)
					newClient(serverSocket);
				else
				{
					handleRequest(i);
				}
			}
		}
	}
}

std::string Server::getBotResponse(const std::string& msg)
{
	std::string response;

	if (msg == "Hello")
		response = "Hello, how can I help you?";
	else if (msg == "Tell me about you")
		response = "I'm a simple irc bot that can answer some of your questions.";
	else
		response = "Sorry, I don't understand your question. Try again.";

	response += "\n";

	return response;
}