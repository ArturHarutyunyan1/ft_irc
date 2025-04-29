#include "../include/Server.hpp"
#include <cerrno>
#include <sys/poll.h>
#include "../include/Bot.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/socket.h>

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    this->_client_fds = new struct pollfd[MAX_CONNECTIONS];
}

Server::~Server()
{}

int Server::getPort() const
{
    return (this->_port);
}

std::string Server::getPassword() const
{
    return (this->_password);
}

void Server::newClient(int fd)
{
    int newFD = accept(fd, NULL, NULL);
    if (newFD == -1)
    {
        perror("Accept error");
    }
    else
    {
        fcntl(newFD, F_SETFL, O_NONBLOCK);
        bool slotFound = false;
        int slotIndex = this->findFreeFdSlot();

        if (slotIndex != -1)
        {
            this->_client_fds[slotIndex].fd = newFD;
            this->_client_fds[slotIndex].events = POLLIN;
            std::cout << "New client connected ✨" << std::endl;
            std::string welcome = welcomeMessage();
            if (send(newFD, welcome.c_str(), welcome.size(), 0) == -1)
                perror("Send error");
            slotFound = true;
        }

        if (!slotFound)
        {
            std::cerr << "No available slots for new client" << std::endl;
            close(newFD);
        }
    }
}

int Server::findFreeFdSlot()
{
    for (int i = 1; i < MAX_CONNECTIONS; i++)
    {
        if (this->_client_fds[i].fd == -1)
            return i;
    }

    return -1;
}

Bot* Server::findBotBySocket(int socketFd)
{
    for (std::vector<Bot *>::iterator it = _botRequests.begin(); it != _botRequests.end(); ++it)
    {
        if ((*it)->getSocketFd() == socketFd)
        {
            return *it;
        }
    }

    return NULL;
}

void Server::cleanupBot(Bot* bot, int fdIndex)
{
    if (bot)
    {
        bot->cleanup();

        for (std::vector<Bot *>::iterator it = this->_botRequests.begin(); it != this->_botRequests.end(); it++)
        {
            if (*it == bot)
            {
                this->_botRequests.erase(it);
                break;
            }
        }
        delete bot;
    }

    if (fdIndex >= 0 && fdIndex < MAX_CONNECTIONS)
    {
        this->_client_fds[fdIndex].fd = -1;
        this->_client_fds[fdIndex].events = 0;
    }
}

void Server::botInit(Bot *bot, int idx)
{
    int botSocket = this->_client_fds[idx].fd;
 
    bot->setSsl(SSL_new(bot->getSslCtx()));
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
            this->_client_fds[idx].events = POLLIN | POLLOUT;
        else if (ssl_err == SSL_ERROR_WANT_WRITE)
            this->_client_fds[idx].events = POLLIN | POLLOUT;
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
        bot->getResponse() += std::string(buffer);;
        size_t posContent = bot->getResponse().find("\"content\"");
        size_t posLogprobs = bot->getResponse().find("\"logprobs\"");

        if (!(posContent == std::string::npos || posLogprobs == std::string::npos))
        {
            std::string extracted = bot->getResponse().substr(posContent + 11, posLogprobs - posContent - 15);
            std::string response = "bot: " + extracted + ".\n";

            send(bot->getClientFd(), response.c_str(), response.size(), 0);
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

void Server::handleRequestBot(int i)
{
    Bot* bot = findBotBySocket(this->_client_fds[i].fd);

    if (!bot)
    {
        this->_client_fds[i].fd = -1;
        this->_client_fds[i].events = 0;
        return;
    }

    int botSocket = _client_fds[i].fd;

    if (bot->getState() == INIT && this->_client_fds[i].revents & POLLOUT)
        this->botInit(bot, i);
    else if (bot->getState() == HANDSHAKE && _client_fds[i].revents & (POLLIN | POLLOUT))
        this->botHandshake(bot, i);
    else if (bot->getState() == SENDING && _client_fds[i].revents & POLLOUT)
        this->botSending(bot, i);
    else if (bot->getState() == RECEIVING && _client_fds[i].revents & POLLIN)
        this->botReceiving(bot, i);
    else if (_client_fds[i].revents & (POLLERR | POLLHUP))
    {
        std::cerr << "Bot socket error, FD: " << botSocket << ", revents: " << _client_fds[i].revents << std::endl;
        std::string error = "bot: Socket error\n";
        send(bot->getClientFd(), error.c_str(), error.size(), 0);
    }
}

void Server::handleRequest(int i) {

    if (findBotBySocket(this->_client_fds[i].fd))
    {
        handleRequestBot(i);
        return;
    }

    char buffer[100000] = {0};
    size_t bytes;

    bytes = recv(this->_client_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        if (bytes == 0) {
            std::cout << "Client disconnected 🚪" << std::endl;
        } else {
            std::cerr << "Something went wrong" << std::endl;
        }
        close(this->_client_fds[i].fd);
        this->_client_fds[i].fd = -1;
        this->_client_fds[i].events = 0;
        return;
    }
    buffer[bytes] = '\0';
    Requests req(buffer, this->_client_fds, this->_client_fds[i].fd, getPassword(), false, this);
    req.handleRequest();
}

void Server::addUser(const std::string &nickname, int fd) {
    std::map<std::string, int>::iterator it = _usernameToFd.find(nickname);

    if (it != _usernameToFd.end()) {
        _usernameToFd.erase(it);
    }
    _usernameToFd[nickname] = fd;
}

void Server::addFd(int fd, const std::string &username) {
    std::map<int, std::string>::iterator it = _fdToUsername.find(fd);

    if (it != _fdToUsername.end()) {
        _fdToUsername.erase(it);
    }
    _fdToUsername[fd] = username;
}

void Server::removeUser(const std::string &nickname, int fd) {
    std::map<std::string, int>::iterator usernameIterator = _usernameToFd.find(nickname);
    std::map<int, std::string>::iterator fdIterator = _fdToUsername.find(fd);

    if (usernameIterator != _usernameToFd.end())
        _usernameToFd.erase(usernameIterator);
    if (fdIterator != _fdToUsername.end())
        _fdToUsername.erase(fdIterator);
}

std::string Server::getNick(int fd) const {
    std::map<int, std::string>::const_iterator it = _fdToUsername.find(fd);

    if (it != _fdToUsername.end())
        return (it->second);
    return ("NULL");
}

int Server::getUser(const std::string &nickname) const {
    std::map<std::string, int>::const_iterator it = _usernameToFd.find(nickname);

    if (it != _usernameToFd.end())
        return (it->second);
    return (-1);
}

// bool Server::joinChannel(const std::string &channel, const std::string &nickname) {
//     if (_channels.find(channel) == _channels.end()) {
//         // _channels[channel] = Channel(); Create channel
//     }
//     return _channels[channel].addClient(nickname);
// }

void Server::start()
{
    int serverSocket;
    int opt = 1;

    serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    if (serverSocket == -1) {
        perror("Socket creation failed");
        return;
    }

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(getPort());
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Bind failed");
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, MAX_CONNECTIONS) == -1) {
        perror("Listen failed");
        close(serverSocket);
        return;
    }

    for (int i = 2; i < MAX_CONNECTIONS; i++) {
        this->_client_fds[i].fd = -1;
        this->_client_fds[i].events = 0;
    }
    this->_client_fds[0].fd = serverSocket;
    this->_client_fds[0].events = POLLIN;

    while (1) {
        int poll_count = poll(this->_client_fds, MAX_CONNECTIONS, -1);
        if (poll_count == -1) {
            perror("Poll error");
            exit(1);
        }

        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (this->_client_fds[i].revents & (POLLIN | POLLOUT | POLLERR)) {
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
    Bot* bot = new Bot();

    int fdIndex = findFreeFdSlot();
    if (fdIndex == -1)
    {
        std::string error = "bot: Server busy, try again later\n";
        send(clientFd, error.c_str(), error.size(), 0);
        bot->cleanup();
        delete bot;
        return;
    }

    int botSocket = bot->getSocketFd();

    this->_botRequests.push_back(bot);
    this->_client_fds[fdIndex].fd = botSocket;
    this->_client_fds[fdIndex].events = 0;

    int port = 443;
    std::pair<std::string, std::string> bodyHost = getRequestBody(msg);

    bot->setRequest(bodyHost.first);

    SSL_load_error_strings();
    SSL_library_init();
    bot->setSslCtx(SSL_CTX_new(SSLv23_client_method()));

    if (!bot->getSslCtx())
    {
        perror("SSL_CTX_new");
        return;
    }
    struct hostent *server = gethostbyname(bodyHost.second.c_str());
    if (server == NULL) perror("gethostbyname");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

    int ret = connect(botSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1 && errno != EINPROGRESS)
    {
        perror("connect");
        std::string error = "bot: Connection failed\n";
        send(clientFd, error.c_str(), error.size(), 0);
        bot->cleanup();
        return;
    }

    bot->setClientFd(clientFd);
    bot->setState(INIT);
    _client_fds[fdIndex].events = POLLOUT;
}