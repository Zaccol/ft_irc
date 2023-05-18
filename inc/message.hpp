#pragma once

#include "irc.hpp"

class message{
	private:
		std::string	_text;
		int			_fd;
	public:
		message(std::string text, int fd);
		~message();
		const std::string	&get_text(void);
		const int			&get_fd(void);
};