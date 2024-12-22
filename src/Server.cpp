#include "../include/Server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password)
{}

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

void Server::start()
{
    int serverSocket;
    int clientSocket;
    int opt = 1;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddress;

    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(getPort());
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    clientSocket = accept(serverSocket, nullptr, nullptr);


    std::cout << "Client connected!" << std::endl;
    close(clientSocket);
    std::cout << "Client disconnected." << std::endl;
    
    close(serverSocket);
}