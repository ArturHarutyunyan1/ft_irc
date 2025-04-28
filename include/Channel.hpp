#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <list>

# include "ServerException.hpp"
# include "ChannelClientStatus.hpp"

class Channel {
public:
	Channel(std::string const& creator) throw(std::bad_alloc);
	~Channel(void) throw();

	/*
	Returns:
		CHANNEL_OK - a client was added
		CHANNEL_NOT_ENOUGH_PLACES - a client exceeded a channel's client limit
		CHANNEL_INVALID_KEY - a key given by a client doesn't match with a channel one
		CHANNEL_CLIENT_NOT_INVITED - a channel has an invite-only flag and a client is not invited
		CHANNEL_CLIENT_ALREADY_IN - a client already presents in a channel
	*/
	ChannelClientStatus	addClient(std::string const& nickname, std::string const& key) throw(std::bad_alloc);

	/*
	Removes a client from channel if it exists taking all privileges 
	*/
	void	kickClient(std::string const& nickname) throw();

	void	changeClientNickname(std::string const& old, std::string const& newNick) throw(std::bad_alloc);

	/*
	Checks if a nickname is titled as an operator
	*/
	bool	isOperator(std::string const& nickname) const throw();

	/*
	Checks if a client with a given nickname presents in a channel
	*/
	bool	isClient(std::string const& nickname) const throw();

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

	/*
	Sets a channel's key.
	If key string is empty then a channel's key is not set
	*/
	void	setKey(std::string const& key) throw(std::bad_alloc);

	/*gets a channel's key*/
	std::string const&	getKey(void) const throw();

	/*Marks a nickname as an operator nickname on a channel*/
	void	addOperator(std::string const& nickname) throw(std::bad_alloc);

	/*Marks a nickname as a non-operator nickname on a channel*/
	void	removeOperator(std::string const& nickname) throw();

	/*Sets maximum client count. If a new limit is less than a current client count in a channel,
		ServerException is throwing*/
	void	setClientLimit(int limit) throw(ServerException);
private:
	std::list<std::string>	clients;
	std::list<std::string>	invited;
	std::list<std::string>	ops;
	std::string				topic;
	std::string				key;
	int						clientLimit;
	bool					isInviteOnly;
	bool					isTopicSettableByOp;

	Channel(void) throw();
	Channel(Channel const& other) throw(std::bad_alloc);

	Channel&	operator=(Channel const& other) throw(std::bad_alloc);
};

#endif
