#ifndef SERVEREXCEPTION_HPP
# define SERVEREXCEPTION_HPP

# include <string>

class ServerException : public std::exception {
public:
	ServerException(void) throw(std::bad_alloc);
	ServerException(std::string const& message) throw(std::bad_alloc);
	ServerException(ServerException const& other) throw(std::bad_alloc);
	~ServerException(void) throw();

	ServerException&	operator=(ServerException const& other) throw(std::bad_alloc);

	char const*	what(void) const throw();
private:
	std::string	message;
};

#endif