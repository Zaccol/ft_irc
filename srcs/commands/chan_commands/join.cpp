#include "irc.hpp"

void	server::cmd_join(commande &param){
	std::vector<std::string> 	args;
	std::string					to_send;

	if (get_client_by_fd(param.get_fd()).get_userdone() == false){
		_messages.push_back(message("command not allow: USER not registered yet\n", param.get_fd()));
		return ;
	}
	if (param.get_params().empty()){
		to_send = ":"+_name + " 461 ";
		to_send += param.get_cmd();
		to_send += ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split(param.get_params(), ",");
	if (args.size() == 1 && args.at(0) == "0"){
		get_client_by_fd(param.get_fd()).remove_all_channels_of_user();	// ! remove n est plus dans le client mais dans le server
		return ;
	}
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++)
		if (is_valid_chan_name(param, *it) == EXIT_FAILURE)
			return ;
	for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); it++){
		if (channel_exists(*it) == EXIT_SUCCESS){
			get_client_by_fd(param.get_fd()).add_chan_to_client(get_channel_by_name(*it));
			get_channel_by_name(*it).add_client_to_chan(get_client_by_fd(param.get_fd()));
			sending_replies_for_existing_channel(param.get_fd(), *it);
		}
		else{
			create_a_new_channel(param, *it); 
			sending_replies_for_new_channel(param.get_fd(), *it);
		}
	}
}