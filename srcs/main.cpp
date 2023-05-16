#include <sstream>
#include <iostream>
#include "server.hpp"

// int	server_loop(Server serv);

int	main(int ac, char **av)
{
	std::string			tmp;
	// std::string			password;
	// std::stringstream	ss;
	// int					port;

	if (ac < 3 || ac > 3)
	{
		std::cerr << "Error: wrong number of arguments\n";
		std::cerr << "Usage: ./ircserv port password\n";
		return (1);
	}
	// TODO does it follows the same rules as client password ?
	// ! if so send this to be parsed same as the commands
	// password.append(av[2]);
	tmp.append(av[1]);
	if (tmp.find_first_not_of("0123456789") != std::string::npos)
	{
		std::cerr << "Error: port must be a number\n";
		std::cerr << "Usage: ./ircserv port password\n";
		return (1);
	}

	// ss << tmp;
	// ss >> port;

	server	serv(atoi(av[1]), av[2]);

	server_loop(serv);
	return (0);
}