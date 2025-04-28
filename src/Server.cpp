#include "../include/Server.hpp"
#include <sys/poll.h>
#include "../include/Bot.hpp"
#include <openssl/ssl.h>
#include <openssl/err.h>

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
        for (int i = 1; i < MAX_CONNECTIONS; i++) {
            if (this->_client_fds[i].fd == -1)
            {
                this->_client_fds[i].fd = newFD;
                this->_client_fds[i].events = POLLIN;
                std::cout << "New client connected ✨" << std::endl;
                std::string welcome = welcomeMessage();
                if (send(newFD, welcome.c_str(), welcome.size(), 0) == -1)
                    perror("Send error");
                slotFound = true;
                break;
            }
        }

        if (!slotFound)
        {
            std::cerr << "No available slots for new client" << std::endl;
            close(newFD);
        }
    }
}

void Server::handleRequestBot(int i)
{
    (void)i;
    std::cout << "Bot socket called\n";


}

void Server::handleRequest(int i)
{
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

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    this->bot = Bot();

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

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        this->_client_fds[i].fd = -1;
        this->_client_fds[i].events = 0;
    }
    this->_client_fds[0].fd = serverSocket;
    this->_client_fds[0].events = POLLIN;

    this->_client_fds[1].fd = this->getBot().socketFd;
    this->_client_fds[1].events = POLLIN;

    while (1) {
        int poll_count = poll(this->_client_fds, MAX_CONNECTIONS, -1);
        if (poll_count == -1) {
            perror("Poll error");
            exit(1);
        }

        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (this->_client_fds[i].revents & POLLIN) {
                if (this->_client_fds[i].fd == serverSocket)
                    newClient(serverSocket);
                else if (this->_client_fds[i].fd == this->getBot().socketFd)
                {
                    handleRequestBot(i);
                }
                else
                {
                    handleRequest(i);
                }
            }
        }
    }
}

std::string Server::sendRequestToBot(std::string msg, int botSocket)
{
    int port = 443;
    std::string host = "api.groq.com";
    std::string apiKey = "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";
    std::string endpoint = "/openai/v1/chat/completions";
    std::string contentType = "Content-Type: application/json";
    std::string bearer = "Authorization: Bearer " + apiKey;
    std::string connection = "Connection: close";

    std::string escapedMsg = escapeJson(msg);
    std::string body =
        "{\n"
        "\"model\": \"meta-llama/llama-4-scout-17b-16e-instruct\",\n"
        "\"messages\": [{\"role\": \"user\", \"content\": \"" + escapedMsg + "\"}]\n"
        "}";

    std::string message =
        "POST " + endpoint + " HTTP/1.1\r\n"
        "Host: " + host + "\r\n"
        + contentType + "\r\n"
        + bearer + "\r\n"
        + connection + "\r\n"
        "Content-Length: " + intToStr(body.length()) + "\r\n"
        "\r\n"
        + body;

    // int botSocket = socket(AF_INET, SOCK_STREAM, 0);

    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ssl_ctx) perror("SSL_CTX_new");

    struct hostent *server = gethostbyname(host.c_str());
    if (server == NULL) perror("gethostbyname");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(botSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("connect");

    SSL *conn = SSL_new(ssl_ctx);
    if (!conn) perror("SSL_new");

    SSL_set_tlsext_host_name(conn, host.c_str());
    SSL_set_fd(conn, botSocket);

    int err = SSL_connect(conn);
    if (err != 1)
    {
        ERR_print_errors_fp(stderr);
        perror("SSL_connect");
    }

    int bytes = SSL_write(conn, message.c_str(), message.length());
    if (bytes <= 0) perror("SSL_write");

    SSL_shutdown(conn);
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx);

    return "";
}

Bot &Server::getBot()
{
    return this->bot;
}

std::string Server::getResponseFromBot(std::string msg)
{
    int port = 443;
    std::string host = "api.groq.com";
    std::string apiKey = "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";
    std::string endpoint = "/openai/v1/chat/completions";
    std::string contentType = "Content-Type: application/json";
    std::string bearer = "Authorization: Bearer " + apiKey;
    std::string connection = "Connection: close";

    std::string escapedMsg = escapeJson(msg);
    std::string body =
        "{\n"
        "\"model\": \"meta-llama/llama-4-scout-17b-16e-instruct\",\n"
        "\"messages\": [{\"role\": \"user\", \"content\": \"" + escapedMsg + "\"}]\n"
        "}";

    std::string message =
        "POST " + endpoint + " HTTP/1.1\r\n"
        "Host: " + host + "\r\n"
        + contentType + "\r\n"
        + bearer + "\r\n"
        + connection + "\r\n"
        "Content-Length: " + intToStr(body.length()) + "\r\n"
        "\r\n"
        + body;

    int botSocket = socket(AF_INET, SOCK_STREAM, 0);

    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ssl_ctx) perror("SSL_CTX_new");

    struct hostent *server = gethostbyname(host.c_str());
    if (server == NULL) perror("gethostbyname");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(botSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("connect");

    SSL *conn = SSL_new(ssl_ctx);
    if (!conn) perror("SSL_new");

    SSL_set_tlsext_host_name(conn, host.c_str());
    SSL_set_fd(conn, botSocket);

    int err = SSL_connect(conn);
    if (err != 1)
    {
        ERR_print_errors_fp(stderr);
        perror("SSL_connect");
    }

    int bytes = SSL_write(conn, message.c_str(), message.length());
    if (bytes <= 0) perror("SSL_write");

    char response[8192];
    memset(response, 0, sizeof(response));
    int total = sizeof(response) - 1;
    int received = 0;

    do
    {
        bytes = SSL_read(conn, response + received, total);
        if (bytes <= 0) break;
        received += bytes;
    }
    while (received < total);

    std::string json(response);
    size_t posContent = json.find("\"content\"");
    size_t posLogprobs = json.find("logprobs");

    std::string extracted;

    if (!(posContent == std::string::npos || posLogprobs == std::string::npos))
        extracted = json.substr(posContent + 11, posLogprobs - posContent - 15);

    SSL_shutdown(conn);
    SSL_free(conn);
    SSL_CTX_free(ssl_ctx);

    return extracted;
}