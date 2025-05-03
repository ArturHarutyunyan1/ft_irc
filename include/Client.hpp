#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Server.hpp"
# include <vector>
# include <iostream>
# include <stdio.h>
# include <string>
# include <fcntl.h>
# include <stdlib.h>
# include <ctime>
# include <errno.h>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <sys/poll.h>
# include <netinet/in.h>
# include <set>

class Client {
	private:
		std::string _password;
		std::string _nick;
		std::string _username;
		std::string _realname;
		std::string _ip;
		std::set<std::string> _channels;
	public:
		std::string const& getNick() const;
		std::string const& getUsername() const;
		std::string const& getRealname() const;
		std::string const& getIP() const;
		std::set<std::string> const& getChannels() const;
		bool isAuthenticated() const;

		void setNick(const std::string &name);
		void setUsername(const std::string &name, const std::string &realname);
		void setPassword(const std::string &password);
		void addChannel(const std::string &channelName);

		bool isPasswordSet() const;
		bool isNickSet() const;
		bool isUserSet() const;

		Client(std::string const& ip) throw(std::bad_alloc);
		Client(void) throw(std::bad_alloc);
		~Client();
		Client(const Client &other);

		Client &operator=(const Client &other);
		bool	operator==(Client const& other) const throw();
		bool	operator<(Client const& other) const throw();

};

#endif