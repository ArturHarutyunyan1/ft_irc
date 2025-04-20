#include "../include/Server.hpp"

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

void Server::handleRequest(int i)
{
    char buffer[1024] = {0};
    size_t bytes;

    bytes = recv(this->_client_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
    buffer[bytes] = '\0';
    Requests req(buffer, this->_client_fds, i, getPassword(), false);
    req.handleRequest();
}

void Server::start()
{
    int serverSocket;
    int opt = 1;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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
                else
                    handleRequest(i);
            }
        }
    }
}   
