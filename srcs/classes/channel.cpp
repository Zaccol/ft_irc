#include "irc.hpp"

channel::channel(std::string name): _name(name), _topic(""), _invite_only(false), _limit_users(0) , _topic_restrict(true), _password("")
{
	// std::vector<client> user_list;
}

channel::~channel()
{
	user_list.erase(user_list.begin(), user_list.end());
	op_user_list.erase(op_user_list.begin(), op_user_list.end());
}

const std::string	&channel::get_name(void)
{
	return (_name);
}

void	channel::set_name(std::string name)
{
	_name = name;
}

const std::string	&channel::get_topic(void)
{
	return (_topic);
}

void	channel::set_topic(std::string topic)
{
	if (topic == ""){
		_topic.clear();
		return ;
	}
	_topic = topic;
}

void	channel::set_invite_only_true(){
	_invite_only = true;
}

void	channel::set_invite_only_false(){
	_invite_only = false;
}

void	channel::set_topic_restrict_true(){
	_topic_restrict = true;
}

void	channel::set_topic_restrict_false(){
	_topic_restrict = false;
}

void	channel::set_password(std::string password){
	_password = password;
}

const bool	&channel::get_invite_only(void){
	return (_invite_only);
}

const bool	&channel::get_topic_restrict(void){
	return (_topic_restrict);
}

const std::string	&channel::get_password(void){
	return (_password);
}

int	channel::is_client_in_chan(std::string which_client){
	std::vector<client>::iterator it = user_list.begin();
	std::vector<client>::iterator ite = user_list.end();

	for (; it != ite; it++)
		if ((*it).get_nickname() == which_client)
			return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

int	channel::is_client_operator(std::string which_client){
	for (std::vector<client>::iterator it = op_user_list.begin(); it != op_user_list.end(); it++)
		if ((*it).get_nickname() == which_client)
			return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

void	channel::add_client_to_chan(client &user){
	user_list.push_back(user);
}

void	channel::add_op_client_to_chan(client &user){
	op_user_list.push_back(user);
}

std::string	channel::list_of_clients_nickname(void){
	std::string	list;

	for (std::vector<client>::iterator it = user_list.begin(); it != user_list.end(); it++){
		if (it == user_list.begin()){
			list = "@"+(*it).get_nickname()+" ";
		}
		else{
			if (is_client_operator(_name) == EXIT_SUCCESS)
				list += "@";
			list += (*it).get_nickname()+" ";
		}
	}
	list.erase(list.end()-1);
	return list;
}

std::vector<message>	channel::send_message_to_all_users_of_this_channel(std::string msg, std::string not_this_client){
	std::vector<message> sendi;

	for (std::vector<client>::iterator it = user_list.begin(); it != user_list.end(); it++){
		if ((*it).get_nickname() != not_this_client){
			sendi.push_back(message(msg, (*it).get_socket()));
		}
	}
	return sendi;
}

size_t	channel::get_nb_users(void){
	return user_list.size();
}

void	channel::remove_client_from_chan(client &user){
	std::vector<client>::iterator it = user_list.begin();
	std::vector<client>::iterator ite = user_list.end();
	
	for (; it != ite; it++){
		if ((*it).get_nickname() == user.get_nickname())
			user_list.erase(it);
	}

	std::vector<client>::iterator it2 = op_user_list.begin();
	std::vector<client>::iterator ite2 = op_user_list.end();

	for (; it2 != ite2; it2++){
		if ((*it2).get_nickname() == user.get_nickname())
			op_user_list.erase(it2);
	}
}

std::vector<std::string>	channel::send_rpl_whoreply_without_mode(std::string _name, std::string chan_name){
	std::vector<std::string>	list;
	std::string					to_send;

	for (std::vector<client>::iterator it = user_list.begin(); it != user_list.end(); it++){
		to_send = ":ft_irc_all 352 " + (*it).get_nickname()+" "+chan_name+" ~"+(*it).get_username()+" "+_name+" " + _name + " " + (*it).get_nickname() + " H :0 " + (*it).get_realname()+"\r\n";
		list.push_back(to_send);
	}
	return list;
}

std::vector<std::string>	channel::send_rpl_whoreply_with_mode(std::string _name, std::string chan_name){
	std::vector<std::string>	list;
	std::string					to_send;

	for (std::vector<client>::iterator it = op_user_list.begin(); it != op_user_list.end(); it++){
		to_send = ":ft_irc_all 352 " + (*it).get_nickname()+" "+chan_name+" ~"+(*it).get_username()+" "+_name+" " + _name + " " + (*it).get_nickname() + " H :0 " + (*it).get_realname()+"\r\n";
		list.push_back(to_send);
	}
	return list;
}

std::vector<message>	channel::warning_users_topic_changed(std::string who_did, std::string server_name, std::string new_topic){
	std::string	to_send;
	std::vector<message>	list;

	for (std::vector<client>::iterator it = user_list.begin(); it != user_list.end(); it++){
		to_send = ":"+who_did+"!~"+(*it).get_username()+"@"+server_name+" TOPIC "+_name+" :"+new_topic+"\r\n";
		list.push_back(message(to_send, (*it).get_socket()));
	}
	return list;
}

std::vector<message>	channel::send_part_msg_to_everyone(std::string msg){
	std::vector<message> list;

	for (std::vector<client>::iterator it = user_list.begin(); it != user_list.end(); it++){
		list.push_back(message(msg, (*it).get_socket()));
	}
	return list;
}

std::vector<message>	channel::send_join_msg_to_everyone(std::string msg){
	std::vector<message> list;

	for (std::vector<client>::iterator it = user_list.begin(); it != user_list.end(); it++){
		list.push_back(message(msg, (*it).get_socket()));
	}
	return list;
}

int	channel::is_chan_empty(void){
	if (user_list.empty())
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

void	channel::remove_password(void){
	_password.clear();
}

void	channel::remove_privilege_from_chan(client &user){
	std::vector<client>::iterator it2 = op_user_list.begin();
	std::vector<client>::iterator ite2 = op_user_list.end();

	for (; it2 != ite2; it2++){
		if ((*it2).get_nickname() == user.get_nickname())
			op_user_list.erase(it2);
	}
}

int	&channel::get_limit_users(void){
	return _limit_users;
}

void	channel::set_limit_users(int nb){
	_limit_users = nb;
}

// int	channel::common_channels(client &querry_user, client &random_user){
// 	//TODO parcourir tous les channels du random_user et si querry appartient a l un d entre eux faut pas le lister dans WHO
// }
