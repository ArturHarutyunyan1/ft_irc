#ifndef ROUTER_HPP
# define ROUTER_HPP

# include <string>
# include <map>

# include "ServerException.hpp"

class Router {
public:
	Router(void) throw(ServerException);
	Router(unsigned short port) throw(ServerException);

	void		start(void) throw(ServerException);
	void		sendToClient(std::string const& nickname, std::string const& message) throw(ServerException);
	std::string	receive(void) const throw(std::bad_alloc);
private:
	std::map<std::string, int>	clientSockets;
	int							serverSocket;

	Router(Router const& other) throw(std::bad_alloc, ServerException);
	~Router(void) throw();

	Router&	operator=(Router const& other);
};

#endif
