#include "irc.hpp"

void	server::cmd_pass(commande &param){
	std::string	to_send;
	
	if (get_client_by_fd(param.get_fd()).get_userdone() == true){
		return ;
	}
	if (param.get_params().empty()){
		to_send = ":"+_name+" 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_passdone() == true){
		to_send = ":"+_name+ " 462 " + get_client_by_fd(param.get_fd()).get_nickname()+ERR_ALREADYREGISTRED;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (param.get_params() != _pass){
		to_send = ":"+_name+ " 464 " + get_client_by_fd(param.get_fd()).get_nickname()+ERR_PASSWDMISMATCH;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	get_client_by_fd(param.get_fd()).set_passdone_true();
}