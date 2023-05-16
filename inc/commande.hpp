#pragma once

#include "irc.hpp"

class commande{
	private:
		std::string	_params;
		int			_fd;
		std::string	_cmd;
		// std::string	_prefix;
		// std::string	_reply;
	public:
		commande();
		commande(std::string params, int fd, std::string cmd);
		const std::string	&get_params(void);
		~commande();
		const std::string	&get_cmd(void);
		// const std::string	&get_prefix(void);
		// const std::string	&get_reply(void);
		const int			&get_fd(void);

		void				set_params(std::string params);
		void				set_cmd(std::string commande);
		// void				set_prefix(std::string prefix);
		// void				set_reply(std::string reply);
		void				set_fd(int socket);
};
