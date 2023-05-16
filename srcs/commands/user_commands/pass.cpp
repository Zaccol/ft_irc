#include "irc.hpp"

void	server::cmd_pass(commande &param){
	std::string	to_send;
	
	if (param.get_params().empty()){
		to_send = "461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_passdone() == true){
		_messages.push_back(message(ERR_ALREADYREGISTRED, param.get_fd()));
		return ;
	}
	if (param.get_params() != _pass){
		_messages.push_back(message(ERR_PASSWDMISMATCH, param.get_fd()));
		return ;	//	il faut trouver un moyen pour empecher l utilisateur de faire quoique se soit si cette erreur est renvoye
	}
	//	on peut verifier ce bool dans chaque fonction ou dans le parsing plus simple ^^ ;-)
	get_client_by_fd(param.get_fd()).set_passdone_true();
}