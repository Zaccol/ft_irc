#include "irc.hpp"

static std::string	get_comment(std::string args){
	std::string	comment = "";
	int 		count = 0;

	for (size_t i = 0; args[i]; i++){
		if (args[i] == ' ')
			count++;
		if (count == 2){
			for (; args[i];++i){
				comment += args[i];
			}
			break ;
		}
	}
	comment.erase(comment.begin());
	return comment;
}

static std::string	get_chans(std::string args){
	std::string	chans;

	for (size_t i = 0; args[i]; i++){
		if (args[i] == ' '){
			for (size_t j = 0; j < i; j++){
				chans += args[j];
			}
			break ;
		}
	}
	return chans;
}

static std::string	get_users(std::string args){
	std::string	users;
	int			count = 0;
	size_t		begin = 0;
	size_t		end = args.size();

	for (size_t i = 0; args[i]; i++){
		if (args[i] == ' ')
			count++;
		if (count == 1){
			begin = i + 1;
			count++;
		}
		else if (count == 3){
			end = i;
			break ;
		}
	}
	for (size_t i = begin; i < end; i++){
		users += args[i];
	}
	return users;
}

void	server::cmd_kick(commande &param){
	std::string					to_send;
	std::string					comment = "";
	std::vector<std::string>	args;
	std::string					list_chans;
	std::string					list_users;
	std::vector<std::string>	chans;
	std::vector<std::string>	users;
	std::string					cpy_params = param.get_params();

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	args = ft_split(param.get_params(), " ");
	if (args.size() < 2){
		to_send = ":"+_name+ " 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
	}
	if (args.size() > 2)
		comment = get_comment(cpy_params);
	list_users = get_users(cpy_params);
	list_chans = get_chans(cpy_params);
	users = ft_split(list_users, ",");
	chans = ft_split(list_chans, ",");
	if (users.size() > 1 && chans.size() > 1){
		to_send = "referring to RFC2812: \"The server MUST NOT send KICK messages with multiple channels or users to clients.\"\r\n";
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (chans.size() > 1){
		for (std::vector<std::string>::iterator it = chans.begin(); it != chans.end(); it++){
			if (channel_exists(*it) == EXIT_FAILURE){
				to_send =  ":"+_name+" 403 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
				to_send += list_chans;
				to_send += ERR_NOSUCHCHANNEL;
				_messages.push_back(message(to_send, param.get_fd()));
				return ;
			}
			if (get_client_by_fd(param.get_fd()).is_it_tonchan(*it)){
				to_send =  ":"+_name+" 442 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
				to_send += *it;
				to_send += ERR_NOTONCHANNEL;
				_messages.push_back(message(to_send, param.get_fd()));
				return ;
			}
			if (get_channel_by_name(*it).is_client_operator(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_FAILURE){
				to_send =  ":"+_name+" 482 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
				to_send += *it;
				to_send += ERR_CHANOPRIVSNEEDED;
				_messages.push_back(message(to_send, param.get_fd()));
				return ;
			}
			if (client_exists(list_users) == EXIT_SUCCESS){
				if (get_channel_by_name(*it).is_client_in_chan(list_users) == EXIT_FAILURE){
					to_send =  ":"+_name+" 441 "+get_client_by_fd(param.get_fd()).get_nickname()+" "+list_users+" "+(*it)+ERR_USERNOTINCHANNEL;
					_messages.push_back(message(to_send, param.get_fd()));
				}
			}
			else{
				to_send =  ":"+_name+" 401 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
				to_send += list_users;
				to_send += " ";
				to_send += *it;
				to_send += ERR_NOSUCHNICK;
				_messages.push_back(message(to_send, param.get_fd()));
				return ;
			}
			to_send = ":" + get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_name(list_users).get_username()+"@"+_name+" KICK "+(*it)+" "+list_users;
			if (comment != "")
				to_send += " "+ comment;
			to_send += "\r\n";
			std::vector<message> list = get_channel_by_name((*it)).send_part_msg_to_everyone(to_send);
			for (std::vector<message>::iterator ite = list.begin(); ite != list.end(); ite++)
				_messages.push_back((*ite));
			get_client_by_name(list_users).remove_one_channel_of_user((*it));
		}
	}
	else {
		if (channel_exists(list_chans) == EXIT_FAILURE){
			to_send =  ":"+_name+" 403 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += list_chans;
			to_send += ERR_NOSUCHCHANNEL;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;
		}
		if (get_client_by_fd(param.get_fd()).is_it_tonchan(list_chans)){
			to_send =  ":"+_name+" 442 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += list_chans;
			to_send += ERR_NOTONCHANNEL;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;
		}
		if (get_channel_by_name(list_chans).is_client_operator(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_FAILURE){
			to_send =  ":"+_name+" 482 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += list_chans;
			to_send += ERR_CHANOPRIVSNEEDED;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;
		}
		for (std::vector<std::string>::iterator it = users.begin(); it != users.end(); it++){
			if (client_exists(list_users) == EXIT_SUCCESS){
				if (get_channel_by_name(list_chans).is_client_in_chan((*it)) == EXIT_FAILURE){
					to_send =  ":"+_name+" 441 "+get_client_by_fd(param.get_fd()).get_nickname()+" "+(*it)+" "+list_chans+ERR_USERNOTINCHANNEL;
					_messages.push_back(message(to_send, param.get_fd()));
				}
			}
			else{
				to_send =  ":"+_name+" 401 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
				to_send += (*it);
				to_send += ERR_NOSUCHNICK;
				_messages.push_back(message(to_send, param.get_fd()));
				return ;
			}
			to_send = ":" + get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_name((*it)).get_username()+"@"+_name+" KICK "+list_chans+" "+(*it);
			if (comment != "")
				to_send += " "+ comment;
			to_send += "\r\n";
			std::vector<message> list = get_channel_by_name(list_chans).send_part_msg_to_everyone(to_send);
			for (std::vector<message>::iterator ite = list.begin(); ite != list.end(); ite++)
				_messages.push_back((*ite));
			get_client_by_name((*it)).remove_one_channel_of_user(list_chans);
		}
	}
}