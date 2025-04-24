#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>

# include "Router.hpp"
# include "Channel.hpp"
# include "ServerException.hpp"

class Server {
public:
	Server(void) throw(ServerException);
	Server(unsigned short port, std::string const& password) throw(ServerException);
	~Server(void) throw();

	void	run(void) throw(std::bad_alloc, ServerException);
private:
	std::map<std::string, Channel>	channels;
	Router							router;

	Server(Server const& other) throw(std::bad_alloc, ServerException);

	Server&	operator=(Server const& other) throw(std::bad_alloc, ServerException);
};

#endif
