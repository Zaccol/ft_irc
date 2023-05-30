#include "irc.hpp"

void	server::cmd_privmsg(commande &param){
	std::vector<std::string>	args;
	std::string					to_send;

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		to_send = ERR_NORECIPIENT;
		to_send += param.get_cmd();
		to_send += ")\r\n";
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split_once(param.get_params(), " ");
	if (args.size() == 1){
		for (size_t i = 0; i < args.size(); i++){
			if (args[i] == " "){
				to_send = ":"+_name+ERR_NORECIPIENT;
				to_send += param.get_cmd();
				to_send += ")\r\n";
				_messages.push_back(message(to_send, param.get_fd()));
				return ;
			}
		}
		to_send = ERR_NOTEXTTOSEND;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (client_exists(args[0]) == EXIT_FAILURE && channel_exists(args[0]) == EXIT_FAILURE){
		to_send = ":"+_name+" 401 ";
		to_send += get_client_by_fd(param.get_fd()).get_nickname();
		to_send += " ";
		to_send += args[0];
		to_send += ERR_NOSUCHNICK;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (client_exists(args[0]) == EXIT_SUCCESS){
		to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+args[0]+"@"+_name+" "+param.get_cmd()+" "+args[0]+" "+args[1]+"\r\n";
		// to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" "+param.get_cmd()+" "+args[0]+" "+args[1]+"\r\n";
		_messages.push_back(message(to_send, get_client_by_name(args[0]).get_socket()));
	}
	if (channel_exists(args[0]) == EXIT_SUCCESS){
		to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" "+param.get_cmd()+" "+args[0]+" "+args[1]+"\r\n";
		std::vector<message> sending_users;
		sending_users = get_channel_by_name(args[0]).send_message_to_all_users_of_this_channel(to_send, get_client_by_fd(param.get_fd()).get_nickname());
		for (std::vector<message>::iterator it = sending_users.begin(); it != sending_users.end(); it++)
			_messages.push_back((*it));
	}
}