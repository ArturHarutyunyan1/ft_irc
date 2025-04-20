#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <list>

class Router;

class Channel {
public:
	Channel(void) throw();
	Channel(Channel const& other) throw(std::bad_alloc);
	~Channel(void) throw();

	Channel&	operator=(Channel const& other) throw(std::bad_alloc);

	void	addClient(std::string const& nickname, Router const& router) throw(std::bad_alloc);
	void	removeClient(std::string const& nickname);
	void	setMessage(std::string const& message) throw(std::bad_alloc);
private:
	std::list<std::string>	clients;
	std::string				message;
};

#endif
