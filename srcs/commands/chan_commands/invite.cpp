#include "irc.hpp"

void	server::cmd_invite(commande &param){
	std::vector<std::string>	args;
	std::string					to_send;

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		to_send =  ":"+_name+" 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split(param.get_params(), " ");
	if (args.size() < 2){
		to_send =  ":"+_name+" 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (client_exists(args.at(0)) == EXIT_FAILURE){
		to_send =  ":"+_name+" 401 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
		to_send += args.at(0);
		to_send += ERR_NOSUCHNICK;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).is_it_tonchan(args.at(1)) == EXIT_FAILURE){
		to_send =  ":"+_name+" 442 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
		to_send += args.at(1);
		to_send += ERR_NOTONCHANNEL;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (get_channel_by_name(args.at(1)).is_client_in_chan(args.at(0)) == EXIT_SUCCESS){
		to_send =  ":"+_name+" 443 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
		to_send += args.at(0);
		to_send += " ";
		to_send += args.at(1);
		to_send += ERR_USERONCHANNEL;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	// if (channel_exists(args.at(1)) == EXIT_FAILURE)
	// 	return ;


	to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" INVITE "+args[0]+" :"+args[1]+"\r\n";
	_messages.push_back(message(to_send, get_client_by_name(args[0]).get_socket()));
	//	RPL_INVITING
	to_send =  ":"+_name+" 341 " + get_client_by_fd(param.get_fd()).get_nickname()+" ";
	to_send += args.at(0);
	to_send += " ";
	to_send += args.at(1);
	to_send += "\r\n";
	_messages.push_back(message(to_send, param.get_fd()));
	get_client_by_name(args.at(0)).add_chan_to_client(get_channel_by_name(args.at(1)));
	get_channel_by_name(args.at(1)).add_client_to_chan(get_client_by_name(args.at(0)));
	sending_replies_for_existing_channel(get_client_by_name(args[0]).get_socket(), args[1]);
}