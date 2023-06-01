#include "irc.hpp"

static std::string	get_chans(std::string args){
	std::string	chans;

	for (size_t i = 0; args[i]; i++){
		if (args[i] == ' '){
			for (size_t j = 0; j < i; j++){
				chans += args[j];
			}
			break ;
		}
		else if (!args[i+1]){
			for (size_t j = 0; j < i+1; j++){
				chans += args[j];
			}
			break ;
		}
	}
	return chans;
}

static std::string	get_keys(std::string args){
	std::string	keys;
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
		keys += args[i];
	}
	return keys;
}

void	server::cmd_join(commande &param){
	// std::vector<std::string> 	args;
	std::string					to_send;
	std::string chans = get_chans(param.get_params());
	std::string keys = get_keys(param.get_params());
	std::vector<std::string>	list_chans;
	std::vector<std::string>	list_keys;

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		to_send = ":"+_name + " 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	// std::cout << param.get_params() << std::endl;
	// std::cout << chans << std::endl;
	list_chans = ft_split(chans, ",");
	list_keys = ft_split(keys, ",");
	if (list_keys.size() != list_chans.size())
		return ;
	if (chans == "0"){
		if (get_client_by_fd(param.get_fd()).get_nb_channels() == 0)
			return ;
		to_send = "PART " + get_client_by_fd(param.get_fd()).part_list_channels_of_user();
		commande cmd = parsing_commands(to_send, param.get_fd());
		cmd_part(cmd);
		return ;
	}
	std::vector<std::string>::iterator it2 = list_chans.begin();
	std::vector<std::string>::iterator ite2 = list_chans.end();

	for (; it2 != ite2; it2++)
		if (is_valid_chan_name(param, *it2) == EXIT_FAILURE)
			return ;
	std::vector<std::string>::iterator it = list_chans.begin();
	std::vector<std::string>::iterator ite = list_chans.end();
	std::vector<std::string>::iterator iter = list_keys.begin();

	// for (; it != ite; it++){
	while (it != ite){
		if (channel_exists(*it) == EXIT_SUCCESS){
			if (get_channel_by_name(*it).is_client_in_chan(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_SUCCESS)
				continue ;
			if (get_channel_by_name((*it)).get_invite_only() == true){
				_messages.push_back(message(ERR_INVITEONLYCHAN(get_client_by_fd(param.get_fd()).get_nickname(), *it), param.get_fd()));
				return ;
			}
			if (get_channel_by_name((*it)).get_limit_users() != 0){
				if (static_cast<size_t>(get_channel_by_name((*it)).get_limit_users()) == get_channel_by_name((*it)).get_nb_users()){
					_messages.push_back(message(ERR_CHANNELISFULL(get_client_by_fd(param.get_fd()).get_nickname(), *it), param.get_fd()));
					return ;
				}
			}
			if (!get_channel_by_name((*it)).get_password().empty()){
				if (keys.empty()){
					_messages.push_back(message(ERR_BADCHANNELKEY(get_client_by_fd(param.get_fd()).get_nickname(), *it), param.get_fd()));
					return ;
				}
				if (get_channel_by_name((*it)).get_password() != (*iter)){
					_messages.push_back(message(ERR_BADCHANNELKEY(get_client_by_fd(param.get_fd()).get_nickname(), *it), param.get_fd()));
					return ;
				}
			}
			get_client_by_fd(param.get_fd()).add_chan_to_client(get_channel_by_name(*it));
			get_channel_by_name(*it).add_client_to_chan(get_client_by_fd(param.get_fd()));
			sending_replies_for_existing_channel(param.get_fd(), *it);
		}
		else{
			sending_replies_for_new_channel(param.get_fd(), *it);
			create_a_new_channel(param, *it);
		}
		it++;
		iter++;
	}
}