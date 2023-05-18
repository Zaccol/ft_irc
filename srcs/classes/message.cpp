#include "irc.hpp"

message::message(std::string text, int fd): _text(text), _fd(fd){}

message::~message(){}

const std::string	&message::get_text(void)
{
	return (_text);
}

const int	&message::get_fd(void)
{
	return (_fd);
}