#include "irc.hpp"

void	server::cmd_who(commande &param){
	std::vector<std::string>	args;
	std::vector<std::string>	rpl_list;
	std::string					to_send;


	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		// std::vector<channel>::iterator ite = _full_channel_list.begin();
		// std::vector<channel>::iterator iteri = _full_channel_list.end();
		// std::vector<client>::iterator it = _full_client_list.begin();
		// std::vector<client>::iterator iter = _full_client_list.end();
		// for (; it != iter; it++){
		// 	if ((*it).get_nickname() != get_client_by_fd(param.get_fd()).get_nickname()){
		// 		for (; ite != iteri; ite++){
		// 			if ((*ite).is_client_in_chan((*it).get_nickname()) == EXIT_SUCCESS && (*ite).is_client_in_chan(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_SUCCESS)
		// 				break ;
		// 		}
		// 		if (ite != _full_channel_list.end()){
		// 			// ! il faut le lister enfin RPL_WHOREPLY puis RPL_ENDOFWHO
		// 			to_send = ":"+_name+" 352 " + (*it).get_nickname()+" "+(*ite).get_name()+" ~"+(*it).get_username()+" "+_name+" " + _name + " " + (*it).get_nickname() + " H@ :0 " + (*it).get_realname()+"\r\n";
		// 			_messages.push_back(message(to_send, param.get_fd()));
		// 		}
		// 	}
		// }
		// to_send = ":"+_name+" 315 " + get_client_by_fd(param.get_fd()).get_nickname() + RPL_ENDOFWHO;
		// _messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	else{
		args = ft_split(param.get_params(), " ");
		if (client_exists(args[0]) == EXIT_FAILURE && channel_exists(args[0]) == EXIT_FAILURE){
			to_send = ":"+_name+" 315 " + get_client_by_fd(param.get_fd()).get_nickname() + " " + args[0] + " " + RPL_ENDOFWHO;
			_messages.push_back(message(to_send, param.get_fd()));
			return ;
		}
		if (args.size() == 1){
			if (client_exists(args[0]) == EXIT_SUCCESS){
				to_send = ":"+_name+" 352 " + get_client_by_fd(param.get_fd()).get_nickname()+" "+get_client_by_fd(param.get_fd()).get_last_channel()+" ~"+get_client_by_fd(param.get_fd()).get_username()+" "+_name+" " + _name + " " + args[0] + " H@ :0 " + get_client_by_fd(param.get_fd()).get_realname()+"\r\n";
				_messages.push_back(message(to_send, param.get_fd()));
				to_send = ":"+_name+" 315 " + get_client_by_fd(param.get_fd()).get_nickname() + " " + args[0] + RPL_ENDOFWHO;
				_messages.push_back(message(to_send, param.get_fd()));
			}
			else if (channel_exists(args[0]) == EXIT_SUCCESS){
				rpl_list = get_channel_by_name(args[0]).send_rpl_whoreply_without_mode(_name, args[0]);
				for (std::vector<std::string>::iterator it = rpl_list.begin(); it != rpl_list.end(); it++){
					_messages.push_back(message((*it), param.get_fd()));
				}
				to_send = ":"+_name+" 315 " + get_client_by_fd(param.get_fd()).get_nickname() + " " + args[0] + RPL_ENDOFWHO;
				_messages.push_back(message(to_send, param.get_fd()));
			}
		}
		else if (args.size() >= 2 && args[1] == "o"){
			if (channel_exists(args[0]) == EXIT_SUCCESS){
				rpl_list = get_channel_by_name(args[0]).send_rpl_whoreply_with_mode(_name, args[0]);
				for (std::vector<std::string>::iterator it = rpl_list.begin(); it != rpl_list.end(); it++){
					_messages.push_back(message((*it), param.get_fd()));
				}
			}
			to_send = ":"+_name+" 315 " + get_client_by_fd(param.get_fd()).get_nickname() + " " + args[0] + RPL_ENDOFWHO;
			_messages.push_back(message(to_send, param.get_fd()));
		}
	}
}