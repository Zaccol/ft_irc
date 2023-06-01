#include "irc.hpp"

void	server::cmd_quit(commande &param){
	std::string	to_send;
	std::vector<client>::iterator it = _full_client_list.begin();
	std::vector<client>::iterator ite = _full_client_list.end();
	std::vector<channel>::iterator iter = _full_channel_list.begin();
	std::vector<channel>::iterator iteri = _full_channel_list.end();

	for (;iter != iteri; iter++){
		if ((*iter).is_client_in_chan(get_client_by_fd(param.get_fd()).get_nickname()) == EXIT_SUCCESS){
			to_send = "PART " + (*iter).get_name();
			commande cmd = parsing_commands(to_send, param.get_fd());
			cmd_part(cmd);
		}
	}
	to_send = ":"+get_client_by_fd(param.get_fd()).get_nickname()+"!~"+get_client_by_fd(param.get_fd()).get_username()+"@"+_name+" "+param.get_cmd();
	if (!param.get_params().empty())
		to_send += " "+param.get_params();
	to_send += "\r\n";
	std::cout << to_send << std::endl;
	send(param.get_fd(), to_send.c_str(), to_send.size(), 0);
	// _messages.push_back(message(to_send, param.get_fd()));
	for (; it != ite; it++){
		if ((*it).get_nickname() == get_client_by_fd(param.get_fd()).get_nickname()){
			_full_client_list.erase(it);
			break ;
		}
	}
	close(param.get_fd());
}