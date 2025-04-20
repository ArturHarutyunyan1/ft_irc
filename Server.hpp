#ifndef SERVER_HPP
# define SERVER_HPP

# include <string>
# include <map>

# include "Router.hpp"
# include "Channel.hpp"

class Server {
public:
	class ServerException : public std::exception {
	public:
		ServerException(void) throw(std::bad_alloc);
		ServerException(ServerException const& other) throw(std::bad_alloc);
		~ServerException(void) throw();

		ServerException&	operator=(ServerException const& other) throw(std::bad_alloc);

		char const*	what(void) const throw();
	private:
		std::string	message;
	};

	Server(void) throw();
	Server(unsigned short port, std::string const& password) throw();
	~Server(void) throw();

private:
	std::map<std::string, Channel>	channels;
	Router							router;

	Server(Server const& other) throw(std::bad_alloc);

	Server&	operator=(Server const& other) throw(std::bad_alloc);
};

#endif
