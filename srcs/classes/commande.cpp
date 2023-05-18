#include "irc.hpp"

commande::~commande(void)
{

}

commande::commande(std::string params, int fd, std::string cmd): _params(params), _fd(fd), _cmd(cmd)
{

}

const std::string	&commande::get_params(void)
{
	return (_params);
}

const std::string	&commande::get_cmd(void)
{
	return (_cmd);
}



// const std::string	&commande::get_reply(void)
// {
// 	return (NULL);
// }

const int	&commande::get_fd(void)
{
	return (_fd);
}

void	commande::set_params(std::string params)
{
	(void)params;
}

void	commande::set_cmd(std::string commande)
{
	(void)commande;
}


// void	commande::set_reply(std::string reply)
// {
// 	(void)reply;
// }

void	commande::set_fd(int socket)
{
	(void)socket;
}
