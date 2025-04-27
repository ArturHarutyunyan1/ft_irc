#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <list>

# include "ServerException.hpp"

class Channel {
public:
	Channel(void) throw();
	~Channel(void) throw();

	/*returns true if a client was added. If a channel is invite-only and the client is not invited
		then false will be returned*/
	bool	addClient(std::string const& nickname) throw(std::bad_alloc);

	/*removes a client from channel if it exists*/
	void	kickClient(std::string const& nickname) throw();

	/*gets a channel's clients*/
	std::list<std::string>::const_iterator	getClients(void) const throw();

	/*adds a client to a channel's invitation list so the client will be added to the channel if the
		channel has invite-only mode*/
	void	inviteClient(std::string const& nickname) throw(std::bad_alloc);

	/*set a channel's welcome message*/
	void	setTopic(std::string const& message) throw(std::bad_alloc);

	/*gets a channel's welcome message*/
	std::string const&	getTopic(void) const throw();

	/*sets or removes invite-only flag*/
	void	setInviteOnly(bool isInviteOnly) throw();

	/*ivite-only flag getter*/
	bool	getInviteOnly(void) const throw();

	/*sets or removes the setting a topic by an operator only restriction*/
	void	setTopicSettableByOp(bool isTopicSettableByOp) throw();

	/*returns the topic settable by an operator only flag*/
	bool	getTopicSettableByOp(void) const throw();

	/*sets a channel's key*/
	void	setKey(std::string const& key) throw(std::bad_alloc);

	/*gets a channel's key*/
	std::string const&	getKey(void) const throw();

	/*Marks a nickname as an operator nickname on a channel*/
	void	addOperator(std::string const& nickname) throw(std::bad_alloc);

	/*Marks a nickname as a non-operator nickname on a channel*/
	void	removeOperator(std::string const& nickname) throw();

	/*Sets maximum client count. If a new limit is less than a current client count in a channel,
		ServerException is throwing*/
	void	setClientLimit(unsigned limit) throw(ServerException);
private:
	std::list<std::string>	clients;
	std::list<std::string>	ops;
	std::string				topic;
	std::string				key;
	unsigned				clientLimit;
	bool					isInviteOnly;
	bool					isTopicSettableByOp;

	Channel(Channel const& other) throw(std::bad_alloc);

	Channel&	operator=(Channel const& other) throw(std::bad_alloc);
};

#endif