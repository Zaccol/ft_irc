#include "irc.hpp"

static int	check_kol(std::string params){
	int	how_many = 0;
	
	for (size_t i = 1; i < params.size(); i++)
		if (params[i] == 'o' || params[i] == 'k' || params[i] == 'l')
			how_many++;
	if (how_many >= 2)
		return EXIT_FAILURE;
	return (EXIT_SUCCESS);
}

static std::string get_param_mods(std::string params){
	std::string mods;
	size_t i = 0;
	for (; i < params.size(); i++){
		if (params[i] == ' ')
			break ;
	}
	i++;
	if (params[i]){
		for (size_t j = i; j < params.size(); j++)
			mods += params[j];
	}
	return mods;
}

void	server::cmd_mode(commande &param){
	std::string	to_send;
	std::vector<std::string> args;

	if (param.get_params().empty()){
		to_send = ":"+ _name + " 461 " + param.get_cmd()+ " " + ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split(param.get_params(), " ");
	if (client_exists(args[0]) == EXIT_SUCCESS)
		return ;
	if (channel_exists(args[0]) == EXIT_FAILURE){
		to_send =  ":"+_name+" 403 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
		to_send += args[0];
		to_send += ERR_NOSUCHCHANNEL;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	// if (get_channel_by_name(args[0]).is_client_operator(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_FAILURE){
	// 	to_send =  ":"+_name+" 482 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
	// 	to_send += args[0];
	// 	to_send += ERR_CHANOPRIVSNEEDED;
	// 	_messages.push_back(message(to_send, param.get_fd()));
	// 	return ;
	// }
	if (args.size() == 1){
		to_send = ":"+ _name + " 324 " + get_client_by_fd(param.get_fd()).get_nickname()+ " " + args[0] + " " + list_active_mods(get_channel_by_name(args[0])) + "\r\n";
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (args[1] == "b")
		return ;
	if (get_channel_by_name(args[0]).is_client_operator(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_FAILURE){
		to_send =  ":"+_name+" 482 "+get_client_by_fd(param.get_fd()).get_nickname()+" ";
		to_send += args[0];
		to_send += ERR_CHANOPRIVSNEEDED;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	std::vector<std::string>::iterator it = args.begin()+1;
	if ((*it).at(0) == '+' || (*it).at(0) == '-'){
		size_t i = (*it).find_first_not_of("ikotl", 1);
		if (i != std::string::npos){
			_messages.push_back(message(ERR_UNKNOWNMODE(get_client_by_fd(param.get_fd()).get_nickname(), (*it).at(i), args[0]), param.get_fd()));
			return ;
		}
	}
	else
		return ;
	if (check_kol(args[1]) == EXIT_FAILURE)
		return ;
	it = args.begin()+1;
	if ((*it).at(0) == '+'){
		for (size_t i = 1; i < args[1].size(); i++){
			if ((*it).at(i) == 'l'){
				if (args.size() < 3)
					return ;
				if (args[2].size() > 1)
					return ;
				if (!(args[2][0] >= '0' && args[2].at(0) <= '9'))
					return ;
				get_channel_by_name(args[0]).set_limit_users(atoi(args[2].c_str()));
			}
			if ((*it).at(i) == 'i'){
				if (get_channel_by_name(args[0]).get_invite_only() == true)
					return ;
				get_channel_by_name(args[0]).set_invite_only_true();
			}
			if ((*it).at(i) == 'o'){
				if (args.size() < 3)
					return ;
				if (client_exists(args[2]) == EXIT_FAILURE){
					to_send =  ":"+_name+" 441 "+get_client_by_fd(param.get_fd()).get_nickname()+" "+args[2]+" "+args[0]+ERR_USERNOTINCHANNEL;
					_messages.push_back(message(to_send, param.get_fd()));
					return ;
				}
				if (client_exists(args[2]) == EXIT_SUCCESS){
					if (get_channel_by_name(args[0]).is_client_in_chan(args[2]) == EXIT_FAILURE){
						to_send =  ":"+_name+" 441 "+get_client_by_fd(param.get_fd()).get_nickname()+" "+args[2]+" "+args[0]+ERR_USERNOTINCHANNEL;
						_messages.push_back(message(to_send, param.get_fd()));
						return ;
					}
				}
				if (get_channel_by_name(args[0]).is_client_operator(args[2]) == EXIT_SUCCESS){
					_messages.push_back(message(RPL_UNIQOPIS(get_client_by_fd(param.get_fd()).get_nickname(), args[0], args[2]), param.get_fd()));
					return ;
				}
				else
					get_channel_by_name(args[0]).add_op_client_to_chan(get_client_by_name(args[2]));
			}
			if ((*it).at(i) == 'k'){
				if (args.size() < 3)
					return ;
				if (!get_channel_by_name(args[0]).get_password().empty()){
					_messages.push_back(message(ERR_KEYSET(get_client_by_fd(param.get_fd()).get_nickname(), args[0]), param.get_fd()));
					return ;
				}
				get_channel_by_name(args[0]).set_password(args[2]);
			}
			if ((*it).at(i) == 't'){
				if (get_channel_by_name(args[0]).get_topic_restrict() == true)
					return ;
				get_channel_by_name(args[0]).set_topic_restrict_true();
			}
		}
	}
	else{
		for (size_t i = 1; i < args[1].size(); i++){
			if ((*it).at(i) == 'l'){
				get_channel_by_name(args[0]).set_limit_users(0);
			}
			if ((*it).at(i) == 'i'){
				if (get_channel_by_name(args[0]).get_invite_only() == false)
					return ;
				get_channel_by_name(args[0]).set_invite_only_false();
			}
			if ((*it).at(i) == 't'){
				if (get_channel_by_name(args[0]).get_topic_restrict() == false)
					return ;
				get_channel_by_name(args[0]).set_topic_restrict_false();
			}
			if ((*it).at(i) == 'k')
				get_channel_by_name(args[0]).remove_password();
			if ((*it).at(i) == 'o'){
				if (args.size() < 3)
					return ;
				if (get_channel_by_name(args[0]).is_client_in_chan(args[2]) == EXIT_FAILURE)
					return ;
				if (get_channel_by_name(args[0]).is_client_operator(args[2]) == EXIT_SUCCESS){
					get_channel_by_name(args[0]).remove_privilege_from_chan(get_client_by_name(args[2]));
				}
			}
		}
	}
	to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" "+ "MODE "+ args[0]+" "+get_param_mods(param.get_params())+"\r\n";
	_messages.push_back(message(to_send, param.get_fd()));
}

// ERR_NEEDMOREPARAMS pour le +k
// ERR_KEYSET pour le +k, il y a deja un mdp
// ERR_CHANOPRIVSNEEDED
// ERR_USERNOTINCHANNEL
// ERR_UNKNOWNMODE

// RPL_CHANNELMODEIS
// RPL_UNIQOPIS	pour le +o, utilisateur deja operateur

// ! faire les modes +-ikot
//	i pour inviteonly
//	k pour key soit les mots de passe pour join un channel apres sa creation
//	o pour les droits d operateurs channel
//	t pour restreindre ou non acces a la commande topic


// void	server::cmd_mode(commande &param){
// 	std::string	to_send;
// 	std::vector<std::string> args;

// 	if (param.get_params().empty()){
// 		to_send = ":"+ _name + " 461 " + param.get_cmd()+ " " + ERR_NEEDMOREPARAMS;
// 		_messages.push_back(message(to_send, param.get_fd()));
// 		return ;
// 	}
// 	args = ft_split(param.get_params(), " ");
// 	if (args.size() == 1){
// 		to_send = ":"+ _name + " 324 " + get_client_by_fd(param.get_fd()).get_nickname()+ " " + args[0] + " +t\r\n";
// 		_messages.push_back(message(to_send, param.get_fd()));
// 		return ;
// 	}
// }