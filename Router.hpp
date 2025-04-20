#ifndef ROUTER_HPP
# define ROUTER_HPP

# include <string>

#include "Server.hpp"

class Router {
public:
	Router(void) throw(Server::ServerException);
	Router(unsigned short port) throw(Server::ServerException);

	void		sendToClient(std::string const& nickname, std::string const& message) throw(Server::ServerException);
	std::string	receive(void) const throw(std::bad_alloc);
private:
	std::map<std::string, int>	clientSockets;
	int							serverSocket;

	Router(Router const& other) throw(std::bad_alloc, Server::ServerException);
	~Router(void) throw();

	Router&	operator=(Router const& other);
};

#endif
