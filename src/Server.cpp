#include "../include/Server.hpp"
#include <cerrno>
#include <string>
#include <sys/poll.h>
#include "../include/Bot.hpp"
#include <sys/socket.h>
#include <algorithm>
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
	for (std::map<int, Bot>::iterator it = _botRequests.begin(); it != _botRequests.end(); it++)
		close(it->first);
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

Bot *Server::findBotBySocket(int socketFd)
{
	std::map<int, Bot>::iterator it = this->_botRequests.find(socketFd);

	if (it != this->_botRequests.end())
		return &(it->second);

	return NULL;
}

void Server::cleanupBot(Bot *bot, int fdIndex)
{
	if (bot)
		bot->cleanup();

	if (fdIndex > 1 && fdIndex < MAX_CONNECTIONS)
	{
		close(this->_client_fds[fdIndex].fd);
		this->_client_fds[fdIndex].fd = -1;
		this->_client_fds[fdIndex].events = 0;
	}
}

void Server::botInit(Bot *bot, int idx)
{
	int botSocket = this->_client_fds[idx].fd;

	SSL_CTX *ctx = bot->getSslCtx();
	SSL *ssl = SSL_new(ctx);

	bot->setSsl(ssl);
	if (!bot->getSsl())
	{
		std::string error = "bot: SSL initialization failed\n";
		send(bot->getClientFd(), error.c_str(), error.size(), 0);
		cleanupBot(bot, idx);
		return;
	}

	SSL_set_tlsext_host_name(bot->getSsl(), "api.groq.com");
	SSL_set_fd(bot->getSsl(), botSocket);
	bot->setState(HANDSHAKE);

	this->_client_fds[idx].events = POLLIN | POLLOUT;
}

void Server::botHandshake(Bot *bot, int idx)
{
	int err = SSL_connect(bot->getSsl());

	if (err == 1)
	{
		bot->setState(SENDING);
		this->_client_fds[idx].events = POLLOUT;
	}
	else
	{
		int ssl_err = SSL_get_error(bot->getSsl(), err);

		if (ssl_err == SSL_ERROR_WANT_READ)
			this->_client_fds[idx].events = POLLIN;
		else if (ssl_err == SSL_ERROR_WANT_WRITE)
			this->_client_fds[idx].events = POLLOUT;
		else
		{
			ERR_print_errors_fp(stderr);
			std::string error = "bot: SSL handshake failed\n";
			send(bot->getClientFd(), error.c_str(), error.size(), 0);
			cleanupBot(bot, idx);
			return;
		}
	}
}

void Server::botSending(Bot *bot, int idx)
{
	int bytes = SSL_write(bot->getSsl(), bot->getRequest().c_str(), bot->getRequest().length());

	if (bytes > 0)
	{
		bot->setState(RECEIVING);
		this->_client_fds[idx].events = POLLIN;
	}
	else
	{
		int ssl_err = SSL_get_error(bot->getSsl(), bytes);

		if (ssl_err == SSL_ERROR_WANT_WRITE)
			this->_client_fds[idx].events = POLLOUT;
		else if (ssl_err == SSL_ERROR_WANT_READ)
			this->_client_fds[idx].events = POLLIN;
		else
		{
			ERR_print_errors_fp(stderr);
			std::string error = "bot: Failed to send request\n";
			send(bot->getClientFd(), error.c_str(), error.size(), 0);
			cleanupBot(bot, idx);
			return;
		}
	}
}

void Server::botReceiving(Bot *bot, int idx)
{
	char buffer[15192];

	int bytes = SSL_read(bot->getSsl(), buffer, sizeof(buffer) - 1);

	if (bytes > 0)
	{
		buffer[bytes] = '\0';
		bot->getResponse() += std::string(buffer);

		size_t posContent = bot->getResponse().find("\"content\"");
		size_t posLogprobs = bot->getResponse().find("\"logprobs\"");

		if (!(posContent == std::string::npos || posLogprobs == std::string::npos))
		{
			std::string extracted = bot->getResponse().substr(posContent + 11, posLogprobs - posContent - 15) + ".\n";

			std::cout << "Bot response for a client with socket #" << bot->getClientFd() << " is ready\n";

			Requests::sendSystemMessage(bot->getClientFd(), bot->getPrefix() + " PRIVMSG " + this->getNick(bot->getClientFd()) + " :" + extracted);
			// send(bot->getClientFd(), response.c_str(), response.size(), 0);
			bot->setState(COMPLETE);
			cleanupBot(bot, idx);
		}
	}
	else if (bytes == 0 || SSL_get_error(bot->getSsl(), bytes) == SSL_ERROR_ZERO_RETURN)
	{
		bot->setState(COMPLETE);
		cleanupBot(bot, idx);
	}
	else
	{
		int ssl_err = SSL_get_error(bot->getSsl(), bytes);

		if (ssl_err == SSL_ERROR_WANT_READ)
			_client_fds[idx].events = POLLIN;
		else if (ssl_err == SSL_ERROR_WANT_WRITE)
			_client_fds[idx].events = POLLOUT;
		else
		{
			perror("SSL_read");
			cleanupBot(bot, idx);
		}
	}
}

void Server::handleRequestBot(Bot *bot, int i)
{
	if (bot->getState() == INIT && this->_client_fds[i].revents & POLLOUT)
		this->botInit(bot, i);
	else if (bot->getState() == HANDSHAKE && _client_fds[i].revents & (POLLIN | POLLOUT))
		this->botHandshake(bot, i);
	else if (bot->getState() == SENDING && _client_fds[i].revents & POLLOUT)
		this->botSending(bot, i);
	else if (bot->getState() == RECEIVING && _client_fds[i].revents & POLLIN)
		this->botReceiving(bot, i);
}

void Server::handleRequest(int i)
{
	Bot *bot = findBotBySocket(this->_client_fds[i].fd);

	if (bot != NULL)
	{
		handleRequestBot(bot, i);
		return;
	}

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
		this->_usernameToFd.erase((_clients[client_fd].getNick()));
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

std::string const Server::getNick(int fd) const
{
	std::map<int, Client>::const_iterator it = _clients.find(fd);

	if (it != _clients.end())
	{
		Client client = it->second;
		return client.getNick();
	}

	return NULL;
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

void Server::sendRequestToBot(std::string msg, int clientFd)
{
	Bot bot = Bot();

	int socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	this->_botRequests[socketFd] = bot;

	if (socketFd < 0)
	{
		perror("socket");
		return;
	}

	int fdIndex = findFreeFdSlot();
	if (fdIndex == -1)
	{
		std::string error = "bot: Server busy, try again later\n";
		send(clientFd, error.c_str(), error.size(), 0);
		bot.cleanup();
		return;
	}

	this->_client_fds[fdIndex].fd = socketFd;

	Bot &botRef = this->_botRequests[socketFd];

	int port = 443;
	std::pair<std::string, std::string> bodyHost = getRequestBody(msg);

	botRef.setRequest(bodyHost.first);
	botRef.setPrefix(":bot!bot");

	SSL_load_error_strings();
	SSL_library_init();
	botRef.setSslCtx(SSL_CTX_new(SSLv23_client_method()));

	if (!botRef.getSslCtx())
	{
		perror("SSL_CTX_new");
		return;
	}
	struct hostent *server = gethostbyname(bodyHost.second.c_str());
	if (server == NULL)
		perror("gethostbyname");

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

	int ret = connect(socketFd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret == -1 && errno != EINPROGRESS)
	{
		perror("connect");
		std::string error = "bot: Connection failed\n";
		send(clientFd, error.c_str(), error.size(), 0);
		botRef.cleanup();
		return;
	}

	botRef.setClientFd(clientFd);
	botRef.setState(INIT);

	this->_client_fds[fdIndex].events = POLLOUT;
}