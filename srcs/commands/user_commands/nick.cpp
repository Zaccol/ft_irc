#include "irc.hpp"

void	server::cmd_nick(commande &param){
	std::string to_send;

	if (get_client_by_fd(param.get_fd()).get_passdone() == false){
		_messages.push_back(message("command not allow: PASS not done yet\r\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		if (get_client_by_fd(param.get_fd()).get_nickname().empty()){
			to_send = ":"+_name+" "+ERR_NONICKNAMEGIVEN;
			_messages.push_back(message(to_send, param.get_fd()));
		}
		else
			_messages.push_back(message(get_client_by_fd(param.get_fd()).get_nickname(), param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_userdone() == false)
		if (handle_errone(param) == EXIT_FAILURE)
			return ;

	if (find_name_occurence(param.get_params(), param.get_fd()) == EXIT_SUCCESS)	
		return ;

	if (get_client_by_fd(param.get_fd()).get_nickname() == param.get_params())
		return ;
	get_client_by_fd(param.get_fd()).set_nickdone_true();
	to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name + " NICK "+param.get_params()+"\r\n";
	_messages.push_back(message(to_send, param.get_fd()));
	get_client_by_fd(param.get_fd()).set_nickname(param.get_params());
}
