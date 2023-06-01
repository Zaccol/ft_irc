#include "irc.hpp"

client::client(std::string name, int socket): _username(name), _socket(socket), _userdone(false), _passdone(false), _nickdone(false)
{
	buf = "";
}

client::client(int socket): _socket(socket), _userdone(false), _passdone(false), _nickdone(false)
{}

client::~client(void)
{
	_channel_list.erase(_channel_list.begin(), _channel_list.end());
}

const std::string	&client::get_nickname(void)
{
	return (_nickname);
}

const std::string	&client::get_username(void)
{
	return (_username);
}

const std::string	&client::get_realname(void)
{
	return (_realname);
}

const std::string	&client::get_hostname(void)
{
	return (_hostname);
}

const bool	&client::get_userdone(void)
{
	return (_userdone);
}

const bool	&client::get_passdone(void)
{
	return (_passdone);
}

const bool	&client::get_nickdone(void)
{
	return (_nickdone);
}

const int	&client::get_socket(void)
{
	return (_socket);
}

void	client::set_nickname(std::string name)
{
	_nickname = name;
}

void	client::set_username(std::string username)
{
	_username = username;
}

void	client::set_realname(std::string realname)
{
	_realname = realname;
}

void	client::set_hostname(std::string hostname)
{
	_hostname = hostname;
}

void	client::set_userdone_true()
{
	_userdone = true;
}

void	client::set_passdone_true()
{
	_passdone = true;
}

void	client::set_nickdone_true()
{
	_nickdone = true;
}

void	client::set_userdone_false()
{
	_userdone = false;
}

void	client::set_passdone_false()
{
	_passdone = false;
}

void	client::set_nickdone_false()
{
	_nickdone = false;
}

int	client::is_it_tonchan(std::string which_chan){
	for (std::vector<channel>::iterator it = _channel_list.begin(); it != _channel_list.end(); it++)
		if ((*it).get_name() == which_chan)
			return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

void	client::add_chan_to_client(channel &chan){
	_channel_list.push_back(chan);
}

std::string	client::part_list_channels_of_user(){
	std::vector<channel>::iterator	it =  _channel_list.begin();
	std::vector<channel>::iterator	ite = _channel_list.end();
	std::string						to_send;

	for (; it != ite; it++){
		to_send += (*it).get_name() + ",";
	}
	if (_channel_list.size() != 0)
		to_send.erase(to_send.end()-1);
	return to_send;
}

void	client::remove_all_channels_of_user(){
	std::vector<channel>::iterator it =  _channel_list.begin();
	std::vector<channel>::iterator ite = _channel_list.end();

	for (; it != ite; it++){
		(*it).remove_client_from_chan(*this);
	}
	// _channel_list.erase(_channel_list.begin(), _channel_list.end());
	_channel_list.clear();
}

void	client::remove_one_channel_of_user(std::string which_chan){
	std::vector<channel>::iterator it =  _channel_list.begin();
	std::vector<channel>::iterator ite = _channel_list.end();

	for (; it != ite; it++){
		if ((*it).get_name() == which_chan){
			// (*it).remove_client_from_chan(*this);
			_channel_list.erase(it);
			return ;
		}
	}
}

channel	&client::get_channel(std::string chan_name){
	std::vector<channel>::iterator it;
	
	for (it = _channel_list.begin(); it != _channel_list.end(); it++){
		if ((*it).get_name() == chan_name)
			break ;
	}
	return (*it);
}

std::string	client::channel_name_list(){
	std::string	channel_list;
	std::vector<channel>::iterator it;

	for (it = _channel_list.begin(); it != _channel_list.end(); it++){
		channel_list += (*it).get_name();
		if (it != _channel_list.end()-1)
			channel_list += ",";
	}
	return channel_list;
}

std::string	client::get_last_channel(){
	if (_channel_list.empty())
		return "*";
	return _channel_list.back().get_name();
}

int	client::get_nb_channels(){
	return _channel_list.size();
}