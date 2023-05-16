#include "irc.hpp"

void	server::cmd_nick(commande &param){
	std::string to_send;

	if (get_client_by_fd(param.get_fd()).get_passdone() == false){
		_messages.push_back(message("command not allow: PASS not done yet\r\n", param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_nickdone() == true){
		return ;
	}
	if (param.get_params().empty()){
		if (get_client_by_fd(param.get_fd()).get_nickname().empty())
			_messages.push_back(message(ERR_NONICKNAMEGIVEN, param.get_fd()));
		else
			_messages.push_back(message(get_client_by_fd(param.get_fd()).get_nickname(), param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_userdone() == false)
		if (handle_errone(param) == EXIT_FAILURE)	// gere le bon format de nick
			return ;
	if (find_name_occurence(param.get_params(), param.get_fd()) == EXIT_SUCCESS)	
		return ;
	get_client_by_fd(param.get_fd()).set_nickname(param.get_params());
	// get_client_by_fd(param.get_fd()).set_userdone_false();	// on ne peut pas faire plusieurs appels a USER sauf si on change le NICK auquel cas on peut remettre a jour les infos dans USER
	// get_client_by_fd(param.get_fd()).set_nickdone_true();	// sert a faire les commandes dans l ordre PASS, NICK, USER
	if (get_client_by_fd(param.get_fd()).get_userdone() == true){
		to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name + " NICK "+param.get_params()+"\r\n";
		_messages.push_back(message(to_send, param.get_fd()));
	}
}

// Le pseudonyme (nickname) est le nom que l'utilisateur choisit pour être identifié dans le réseau IRC.