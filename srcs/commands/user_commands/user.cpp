#include "irc.hpp"

static std::string	get_full_realname(std::vector<std::string> parse){
	std::string rname;

	if (parse[3].find(":") == 0){
		for (std::vector<std::string>::iterator it = parse.begin()+3; it != parse.end(); it++){
			rname += (*it) + " ";
		}
	}
	else
		return (parse[3]);
	rname.erase(rname.end()-1);
	return (rname);
}

void	server::cmd_user(commande &param){
	std::vector<std::string>	args;
	std::string					to_send;

	if (get_client_by_fd(param.get_fd()).get_passdone() == false){
		_messages.push_back(message("command not allow: PASS not done yet\r\n", param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_nickdone() == false){
		_messages.push_back(message("command not allow: NICK not done yet\r\n", param.get_fd()));
		return ;
	}
	if (get_client_by_fd(param.get_fd()).get_userdone() == true){
		to_send = ":"+_name+" 462 " + get_client_by_fd(param.get_fd()).get_nickname() + " " +ERR_ALREADYREGISTRED;
		_messages.push_back(message(to_send, param.get_fd()));
		commande cmd = parsing_commands("QUIT", param.get_fd());
		cmd_quit(cmd);
		return ;
	}
	get_client_by_fd(param.get_fd()).set_userdone_false();
	if (param.get_params().empty()){
		to_send = ":"+_name+" 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split(param.get_params(), " ");
	if (args.size() < 4){
		to_send = ":"+_name+" 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	get_client_by_fd(param.get_fd()).set_username(args.at(0));
	get_client_by_fd(param.get_fd()).set_realname(get_full_realname(args));
	get_client_by_fd(param.get_fd()).set_hostname(HOST);

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
	//	RPL 001
		to_send = ":"+_name + " 001 " + get_client_by_fd(param.get_fd()).get_nickname();
		to_send += RPL_WELCOME;
		to_send += get_client_by_fd(param.get_fd()).get_nickname() + "\r\n";
		_messages.push_back(message(to_send, param.get_fd()));

	//	RPL 002
		to_send = ":"+_name + RPL_YOURHOST(get_client_by_fd(param.get_fd()).get_nickname());
		_messages.push_back(message(to_send, param.get_fd()));

	//	RPL 003
		to_send = ":"+_name + RPL_CREATED(get_client_by_fd(param.get_fd()).get_nickname());
		_messages.push_back(message(to_send, param.get_fd()));
	
	//	RPL 004
		to_send = ":"+_name + RPL_MYINFO(get_client_by_fd(param.get_fd()).get_nickname());
		_messages.push_back(message(to_send, param.get_fd()));
	}
	//	RPL 005

	get_client_by_fd(param.get_fd()).set_userdone_true();


}

// Le nom d'utilisateur (username) est le nom d'utilisateur de l'utilisateur sur le système où il exécute son client IRC.

// Le nom réel (realname) est un champ optionnel permettant à l'utilisateur de fournir une description plus détaillée de lui-même ou de son compte.

// Le nom d'hôte (hostname) est le nom de la machine sur laquelle l'utilisateur exécute son client IRC. Mais on ne s occupe pas de l hostname
// parce qu il est normalement automatiquement enregistre quand on rejoint le server grace a son IP.

//! Nickname (pseudo) :

//? Le nickname est un identifiant unique utilisé par un utilisateur sur le serveur IRC.
//? Il peut être choisi librement par l'utilisateur, mais il doit être unique parmi tous les utilisateurs connectés au serveur.
//? Le nickname est utilisé pour identifier un utilisateur lorsqu'il rejoint des canaux, envoie des messages privés, etc.
//? Le nickname peut être visible aux autres utilisateurs sur le serveur.

//! Username (nom d'utilisateur) :

//? Le username est une autre forme d'identification utilisée sur le serveur IRC.
//? Il est souvent associé à un compte utilisateur enregistré sur le serveur IRC.
//? Contrairement au nickname, le username n'est pas choisi librement par l'utilisateur, mais est généralement déterminé par le serveur ou associé au compte de l'utilisateur.
//? Le username peut être utilisé pour authentifier l'utilisateur lors de la connexion au serveur IRC.
//? Le username n'est pas aussi visible que le nickname et n'est généralement pas utilisé dans les interactions courantes sur le serveur.