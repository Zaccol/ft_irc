#include "irc.hpp"

void	server::cmd_topic(commande &param){
	std::vector<std::string>	args;
	std::string					to_send;

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		to_send = "461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split_once(param.get_params(), " ");
	if (get_client_by_fd(param.get_fd()).is_it_tonchan(args.at(0)) == EXIT_FAILURE){
		to_send = ":"+_name+ " 442 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
		to_send = args.at(0);
		to_send += ERR_NOTONCHANNEL;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (get_channel_by_name(args[0]).get_topic_restrict() == true){
		if (get_channel_by_name(args.at(0)).is_client_operator(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_FAILURE){
			to_send = ":"+_name+" 482 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += args.at(0);
			to_send += ERR_CHANOPRIVSNEEDED;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;
		}
	}
	if (args.size() == 1){
		if (!get_channel_by_name(args.at(0)).get_topic().empty()){
			to_send = ":"+_name+" 332 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += args.at(0);
			to_send += " ";
			to_send += get_channel_by_name(args.at(0)).get_topic();
			to_send += "\r\n";
			_messages.push_back(message(to_send, param.get_fd()));
		}
		else{
			to_send = ":"+_name+" 331 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += args.at(0);
			to_send += RPL_NOTOPIC;
			_messages.push_back(message(to_send, param.get_fd()));
		}
	}
	else{
			get_channel_by_name(args.at(0)).set_topic(args.at(1));
			std::vector<message> sendi;
			sendi = get_channel_by_name(args.at(0)).warning_users_topic_changed(get_client_by_fd(param.get_fd()).get_nickname(), _name, args[1]);
			for (std::vector<message>::iterator it = sendi.begin(); it != sendi.end(); it++){
				_messages.push_back(*it);
			}
	}
}