#include "irc.hpp"

void	server::cmd_quit(commande &param){
	std::string	to_send;

	to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" "+param.get_cmd();
	if (!param.get_params().empty())
		to_send += " "+param.get_params();
	to_send += "\r\n";
	_messages.push_back(message(to_send, param.get_fd()));
	for (std::vector<client>::iterator it = _full_client_list.begin(); it != _full_client_list.end(); it++){
		if ((*it).get_nickname() == get_client_by_fd(param.get_fd()).get_nickname()){
			std::cout << (*it).get_nickname() << std::endl;
			_full_client_list.erase(it);
			break ;
		}
	}
	// close(param.get_fd());
}