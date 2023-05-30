#include "irc.hpp"

void	server::cmd_part(commande &param){
	std::string					to_send;
	std::vector<std::string>	args;
	std::string					comment = "";

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		to_send = ":"+_name+" 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
	}
	args = ft_split_once(param.get_params(), " ");
	if (args.size() > 1){
		comment = args[1];
	}
	args = ft_split(args[0], ",");
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++){
		if (channel_exists(*it) == EXIT_FAILURE){
			to_send = ":"+_name+" 403 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += *it;
			to_send += ERR_NOSUCHCHANNEL;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;	//	on affiche toutes les erreurs ou des qu on en voit une ca stop?
		}
	}
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++){
		if (get_client_by_fd(param.get_fd()).is_it_tonchan(*it) == EXIT_FAILURE){
			to_send = ":"+_name+ " 442 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
			to_send += *it;
			to_send += ERR_NOTONCHANNEL;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;
		}
	}
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++){
		to_send = ":" + get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" PART "+(*it);
		if (comment != "")
			to_send += " "+comment;
		to_send += "\r\n";
		std::vector<message> list = get_channel_by_name((*it)).send_part_msg_to_everyone(to_send);
		for (std::vector<message>::iterator it = list.begin(); it != list.end(); it++)
			_messages.push_back((*it));
		get_client_by_fd(param.get_fd()).remove_one_channel_of_user((*it));
		get_channel_by_name((*it)).remove_client_from_chan(get_client_by_fd(param.get_fd()));
		if (get_channel_by_name((*it)).is_chan_empty() == EXIT_SUCCESS)
			delete_chan((*it));
	}
}