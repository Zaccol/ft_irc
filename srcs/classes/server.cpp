#include "irc.hpp"

#define MAX_CLIENT 10

const std::string &			server::get_name(void){ return _name; }
const std::string &			server::get_pass(void){ return _pass; }
const int &					server::get_port(void){ return _port; }

void						server::set_name(std::string name){ _name = name; }
void						server::set_pass(std::string pass){ _pass = pass; }
void						server::set_port(int port){ _port = port; }

int	server::get_server_fd(void) const{ return (_server_fd); }

int	server::get_max_fd(void)
{
	int	max_fd = _server_fd;

	for (std::vector<client>::iterator it = _full_client_list.begin(); it != _full_client_list.end(); it++)
	{
		if ((*it).get_socket() > max_fd){
			max_fd = (*it).get_socket();
		}
	}
	return (max_fd);
}

server::server(int port, char *password): _name("ft_irc_all"), _pass(password)
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
	{
		std::cerr << "Error: server socket creation failed\n";
		exit(1);
	}
	const int enable = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0){
		std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;
		exit (2);
	}
	// _full_client_list.push_back(client("server", _server_fd));	// ! a voir
	// memset(&server_addr, 0, sizeof(server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_port = htons(port);
	_server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(_server_fd, (struct sockaddr *)&_server_addr, sizeof(_server_addr)) < 0)
	{
		std::cerr << "Error: bind socket to PORT failed\n";
		exit(2);
	}
	if (listen(_server_fd, MAX_CLIENT) < 0)
	{
		std::cerr << "Error: socket listen failed\n";
		exit(3);
	}
}

server::~server(void)
{
	_full_channel_list.erase(_full_channel_list.begin(), _full_channel_list.end());
	_full_client_list.erase(_full_client_list.begin(), _full_client_list.end());
	close(_port);
}

int	server::add_client(void)
{
	socklen_t	client_addr_len = sizeof(_client_addr);
	client new_client(accept(_server_fd, (struct sockaddr *)&_client_addr, &client_addr_len));

	if (new_client.get_socket() < 0)
	{
		std::cerr << "Error: could not accept new client connection to server\n";
		return (1);
	}
	std::cout << "NEW CLIENT\n";
	_full_client_list.push_back(new_client);
	return (0);
}

void	server::del_client(int fd)
{
	get_client_by_fd(fd).remove_all_channels_of_user();
	close(fd);
	std::cout << "BYE BYE CLIENT\n";
}

fd_set	server::ret_read_fds(void)
{
	fd_set	read_fds;

	FD_ZERO(&read_fds);
	FD_SET(_server_fd, &read_fds);
	for (std::vector<client>::iterator it = _full_client_list.begin(); it != _full_client_list.end(); ++it)
	{
		FD_SET((*it).get_socket(), &read_fds);
	}
	return (read_fds);
}

fd_set	server::ret_write_fds(void)
{
	fd_set	write_fds;
	
	FD_ZERO(&write_fds);
	if (_messages.empty() == true)
		return (write_fds);
	for (std::vector<message>::iterator it = _messages.begin(); it != _messages.end(); ++it)
	{
			if (FD_ISSET((*it).get_fd(), &write_fds) == 0)
				FD_SET((*it).get_fd(), &write_fds);
	}
	return (write_fds);
}

// void server::signal_handler(int signal) {
//     if (signal == SIGINT) {
//         // fermeture de la socket serveur
//         close(_port);
//         std::cout << "Server socket closed." << std::endl;
//         exit(signal);
//     }
// }

void	server::send_messages(fd_set write_fds){
	if (!_messages.empty() && !_full_client_list.empty()){
		// std::cout << "\nthere are " << _messages.size() << " messages to be sent\n";
		for (std::vector<message>::iterator it = _messages.begin(); it != _messages.end(); it++){
			if (FD_ISSET((*it).get_fd(), &write_fds)){
				send((*it).get_fd(), (*it).get_text().c_str(), (*it).get_text().size(), 0);
				// FD_CLR((*it).get_fd(), &write_fds);
				// std::cout << "one message" << std::endl;
				// it = _messages.begin();
			}
			// else{
			// 	// ! pas compris ce qu il fallait faire sinon
			// 	// _messages.erase(it);
			// 	// it = _messages.begin();
			// 	std::cout << "pk ca passerait la dedans?" << std::endl;
			// 	;
			// }
		}
		// for (size_t i = 0; i < _messages.size(); i++){
		// 	if (FD_ISSET(_messages[i].get_fd(), &write_fds)){
		// 		send(_messages[i].get_fd(), _messages[i].get_text().c_str(), _messages[i].get_text().size(), MSG_DONTWAIT);
		// 		FD_CLR(_messages[i].get_fd(), &write_fds);
		// 		std::cout << "one message" << std::endl;
		// 		// it = _messages.begin();
		// 	}
		// }
	}
	_messages.erase(_messages.begin(), _messages.end());	// ! apres que les messages aient ete lu il faut les supprimer
}

commande	server::parsing_commands(std::string msg, int fd)
{
	std::string	tmp;
	std::string text;
	std::string cmd;


	if (msg[0] == ':')
	{
		msg.assign(msg.substr(msg.find(' '), msg.size()));
	}
	if (msg.find(' ') != std::string::npos)
	{
		tmp.assign(msg.substr(0, msg.find(' ')));
		cmd = tmp;
		text = msg.substr(msg.find(' ') + 1, msg.size());
		if (text.find('\n') != std::string::npos)
		{
			text.erase(text.find('\n'));
			if (text.find('\r') != std::string::npos)
				text.erase(text.find('\r'));
			// else
		}
		// std::cout << "cmd:" << cmd << "|params:" << text << std::endl;
	}
	else
	{
		cmd = msg;
		if (cmd.find('\n') != std::string::npos)
			cmd.erase(cmd.find('\n') - 1);
	}
	return (commande(text, fd, cmd));
}

void	server::cmd_mode(commande &param){
	std::string	to_send;
	std::vector<std::string> args;

	if (param.get_params().empty()){
		to_send = ":"+ _name + " 461 " + param.get_cmd()+ " " + ERR_NEEDMOREPARAMS;
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	args = ft_split(param.get_params(), " ");
	if (args.size() == 1){
		to_send = ":"+ _name + " 324 " + get_client_by_fd(param.get_fd()).get_nickname()+ " " + args[0] + " +t\r\n";
		_messages.push_back(message(to_send, param.get_fd()));
		return ;
	}
	if (args[1] == "+i"){
		return ;
	}
	if (args[1] == "b"){
		return ;
	}
	// to_send = ":"+ _name + " 324 " + get_client_by_fd(param.get_fd()).get_nickname()+ " " + args[0] + " +t\r\n";
	// _messages.push_back(message(to_send, param.get_fd()));
	// (void)param;
}

void	server::handle_message(std::string line, int fd)
{
	std::string	cmds_str[13] = {"INVITE", "JOIN", "KICK", "PART", "TOPIC", "PRIVMSG", "NOTICE", "NICK", "PASS", "USER", "CAP", "MODE", "WHO"};
	size_t	space_pos = line.find_first_of(' ');

	// std::cout << "on entre dans la gestion des messages" << std::endl;
	if (line[0] == ':')
	{
		// std::cout << "on entre dans le if de la gestion des messages" << std::endl;
		if (space_pos != std::string::npos && space_pos > 1)
		{
			for (int i = 0; i < 13; ++i)
			{
				if (line.find(cmds_str[i]) == space_pos + 1)
				{
					// std::cout << "FOUND A COMMAND : " << cmds_str[i] << '\n';
					commande cmd = parsing_commands(line, fd);
					line.clear();
					if (cmds_str[i].compare("PASS") == 0)
						cmd_pass(cmd);
					else if (cmds_str[i].compare("NICK") == 0)
						cmd_nick(cmd);
					else if (cmds_str[i].compare("USER") == 0)
						cmd_user(cmd);
					else if (cmds_str[i].compare("INVITE") == 0)
						cmd_invite(cmd);
					else if (cmds_str[i].compare("JOIN") == 0)
						cmd_join(cmd);
					else if (cmds_str[i].compare("KICK") == 0)
						cmd_kick(cmd);
					else if (cmds_str[i].compare("PART") == 0)
						cmd_part(cmd);
					else if (cmds_str[i].compare("TOPIC") == 0)
						cmd_topic(cmd);
					else if (cmds_str[i].compare("PRIVMSG") == 0)
						cmd_privmsg(cmd);
					else if (cmds_str[i].compare("NOTICE") == 0)
						cmd_notice(cmd);
					else if (cmds_str[i].compare("CAP") == 0)
						cmd_cap();
					else if (cmds_str[i].compare("MODE") == 0)
						cmd_mode(cmd);
					else if (cmds_str[i].compare("WHO") == 0)
						cmd_who(cmd);
					return ;
				}
			}
		}
	}
	else
	{
		// std::cout << "on entre dans le else de la gestion des messages" << std::endl;
		for (int i = 0; i < 13; ++i)
		{
			if (line.find(cmds_str[i]) == 0)
			{
				// std::cout << "FOUND A COMMAND : " << cmds_str[i] << '\n';
				// create command with prefix
				// send command to command fuction
				// commande	cmd(parsing_commands(line, fd));
				// msg._cmd = cmd;
				commande cmd = parsing_commands(line, fd);
				line.clear();
				if (cmds_str[i].compare("PASS") == 0)
					cmd_pass(cmd);
				else if (cmds_str[i].compare("NICK") == 0)
					cmd_nick(cmd);
				else if (cmds_str[i].compare("USER") == 0)
					cmd_user(cmd);
				else if (cmds_str[i].compare("INVITE") == 0)
					cmd_invite(cmd);
				else if (cmds_str[i].compare("JOIN") == 0)
					cmd_join(cmd);
				else if (cmds_str[i].compare("KICK") == 0)
					cmd_kick(cmd);
				else if (cmds_str[i].compare("PART") == 0)
					cmd_part(cmd);
				else if (cmds_str[i].compare("TOPIC") == 0)
					cmd_topic(cmd);
				else if (cmds_str[i].compare("PRIVMSG") == 0)
					cmd_privmsg(cmd);
				else if (cmds_str[i].compare("NOTICE") == 0)
					cmd_notice(cmd);
				else if (cmds_str[i].compare("CAP") == 0)
					cmd_cap();
				else if (cmds_str[i].compare("MODE") == 0)
					cmd_mode(cmd);
				else if (cmds_str[i].compare("WHO") == 0)
					cmd_who(cmd);
				//* ((this->*command[i])())
				return ;
			}
		}
	}
}

void	server::fct_buffer(fd_set read_fds){
		char		buffer[1024] = {0};
		std::string	tmp;

		for (std::vector<client>::iterator it = _full_client_list.begin(); it != _full_client_list.end(); ++it)
		{
			// std::cout << "on entre dans fct_buffer" << std::endl;
			int	fd = (*it).get_socket();
			if (FD_ISSET(fd, &read_fds))
			{
				int	valread = read(fd, buffer, 512);
				if (valread < 0)
				{
					del_client(fd);
					if (_full_client_list.empty() == true)
						break ;
					it = _full_client_list.begin();
				}
				else
				{
					tmp = buffer;
					std::cout << tmp;
					if (tmp.find('\n') == std::string::npos)
						tmp += "\n";
					while (tmp.find('\n') != std::string::npos)
					{
						handle_message(tmp, fd);
						tmp.assign(tmp.substr(tmp.find('\n') + 1, tmp.size()));
					}
				}
			}
		}
}

// void	server::create_cmds_map(void){
// 	_cmds_map.insert(std::make_pair("invite", &server::cmd_invite));	//	c est dans <utility> donc je comprend pas l erreur
// 	_cmds_map.insert(std::make_pair("join", &server::cmd_join));
// 	_cmds_map.insert(std::make_pair("kick", &server::cmd_kick));
// 	_cmds_map.insert(std::make_pair("part", &server::cmd_part));
// 	_cmds_map.insert(std::make_pair("topic", &server::cmd_topic));
// 	_cmds_map.insert(std::make_pair("notice", &server::cmd_invite));
// 	_cmds_map.insert(std::make_pair("privmsg", &server::cmd_privmsg));
// 	_cmds_map.insert(std::make_pair("nick", &server::cmd_invite));
// 	_cmds_map.insert(std::make_pair("pass", &server::cmd_pass));
// 	_cmds_map.insert(std::make_pair("user", &server::cmd_user));
// }

int	server::find_name_occurence(std::string name, int fd){
	std::string to_send;

	for (std::vector<client>::iterator it = _full_client_list.begin(); it != _full_client_list.end(); it++){
		if (name == (*it).get_nickname() && fd != (*it).get_socket()){
			to_send = "433 ";
			to_send += name;
			to_send += ERR_NICKNAMEINUSE;
			_messages.push_back(message(to_send, fd));
			return EXIT_SUCCESS;
		}
	}
	return (EXIT_FAILURE);
}

client	&server::get_client_by_name(const std::string name){
	std::vector<client>::iterator it;
	
	for (it = _full_client_list.begin(); it != _full_client_list.end(); it++){
		if (name == (*it).get_nickname())
			break ;
	}
	return ((*it));
}

client	&server::get_client_by_fd(const int fd){
	std::vector<client>::iterator it;

	for (it = _full_client_list.begin(); it != _full_client_list.end(); it++){
		if (fd == (*it).get_socket())
			break ;
	}
	return (*it);
}

channel	&server::get_channel_by_name(const std::string name){
	std::vector<channel>::iterator it;

	for (it = _full_channel_list.begin(); it != _full_channel_list.end(); it++){
		if (name == (*it).get_name())
			break ;
	}
	return (*it);
}

std::vector<std::string> server::ft_split(std::string param, std::string delim){
	std::vector<std::string>	res;
	std::string::size_type		found;
		
	while ( (found = param.find(delim)) != std::string::npos){
		if (found != 0)
			res.push_back(param.substr(0, found));
		param = param.substr(found + delim.size(), param.size());
	}
	if (param.size() != 0)
		res.push_back(param);

	return (res);
}

std::vector<std::string> server::ft_split_once(std::string param, std::string delim){
	std::vector<std::string>	res;
	std::string::size_type		found;
		
	if ( (found = param.find(delim)) != std::string::npos){
		if (found != 0)
			res.push_back(param.substr(0, found));
		param = param.substr(found + delim.size(), param.size());
	}
	if (param.size() != 0)
		res.push_back(param);

	return (res);
}

int	server::client_exists(const std::string name){
	std::vector<client>::iterator it;

	for (it = _full_client_list.begin(); it != _full_client_list.end(); it++){
		if (name == (*it).get_nickname())
			return EXIT_SUCCESS;
	}
	return (EXIT_FAILURE);
}

int	server::channel_exists(const std::string name){
	std::vector<channel>::iterator it;

	for (it = _full_channel_list.begin(); it != _full_channel_list.end(); it++){
		if (name == (*it).get_name())
			return EXIT_SUCCESS;
	}
	return (EXIT_FAILURE);
}

void	server::add_chan_to_server(channel &chan){
	_full_channel_list.push_back(chan);
}

void	server::add_client_to_server(client &user){
	_full_client_list.push_back(user);
}

int	server::is_valid_chan_name(commande &param, std::string chan_check){
	std::string to_send;
	
	if (chan_check.size() > 50 || (chan_check[0] != '&' && chan_check[0] != '+' && chan_check[0] != '#' && chan_check[0] != '!')){
		to_send = "403 ";
		to_send += chan_check;
		to_send += ERR_NOSUCHCHANNEL;
		_messages.push_back(message(to_send, param.get_fd()));
		return EXIT_FAILURE;
	}
	for (size_t i = 1; chan_check[i]; i++){
		if (chan_check[i] == ' ' || chan_check[i] == ',' || chan_check[i] == 7){
			to_send = "403 ";
			to_send += chan_check;
			to_send += ERR_NOSUCHCHANNEL;
			_messages.push_back(message(to_send, param.get_fd()));
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

void	server::create_a_new_channel(commande &param, std::string new_channel_name){
	channel new_channel = channel(new_channel_name);

	_full_channel_list.push_back(new_channel);
	get_channel_by_name(new_channel_name).add_client_to_chan((get_client_by_fd(param.get_fd())));
	get_channel_by_name(new_channel_name).add_op_client_to_chan(get_client_by_fd(param.get_fd()));
	get_client_by_fd(param.get_fd()).add_chan_to_client(new_channel);
}

void	server::sending_replies_for_existing_channel(int fd, std::string chan_name){
	std::string to_send;
	std::vector<message> list;

	to_send = ":"+get_client_by_fd(fd).get_nickname()+"!~" +get_client_by_fd(fd).get_username()+"@"+_name+" JOIN :"+chan_name+"\r\n";
	list = get_channel_by_name(chan_name).send_join_msg_to_everyone(to_send);
	for (std::vector<message>::iterator it = list.begin(); it != list.end(); it++)
		_messages.push_back((*it));
	// if (get_channel_by_name(chan_name).get_topic().empty()){
	// 	to_send = ":"+_name+" 331 ";
	// 	to_send += get_client_by_fd(fd).get_nickname() + " ";
	// 	to_send += chan_name;
	// 	to_send += RPL_NOTOPIC;
	// 	_messages.push_back(message(to_send, fd));
	// }
	// else{
	// 	to_send = ":"+_name+" 332 ";
	// 	to_send += get_client_by_fd(fd).get_nickname() + " ";
	// 	to_send += chan_name;
	// 	to_send += " :";
	// 	to_send += get_channel_by_name(chan_name).get_topic()+"\r\n";
	// 	_messages.push_back(message(to_send, fd));
	// }
	to_send = ":"+_name+" 353 = ";
	to_send += get_client_by_fd(fd).get_nickname() + " ";
	to_send += chan_name;
	to_send += " :";
	to_send += get_channel_by_name(chan_name).list_of_clients_nickname();
	to_send += "\r\n";
	_messages.push_back(message(to_send, fd));
	to_send = ":"+_name+" 366 ";
	to_send += get_client_by_fd(fd).get_nickname() + " ";
	to_send += chan_name;
	to_send += RPL_ENDOFNAMES;
	_messages.push_back(message(to_send, fd));
}

void	server::sending_replies_for_new_channel(int fd, std::string chan_name){
	std::string to_send;
	
	to_send = ":"+get_client_by_fd(fd).get_nickname()+"!~" +get_client_by_fd(fd).get_username()+"@"+_name+" JOIN :";
	to_send += chan_name;
	to_send += "\r\n";
	_messages.push_back(message(to_send, fd));
	// to_send = ":"+_name+" 331 ";
	// to_send += get_client_by_fd(fd).get_nickname() + " ";
	// to_send += chan_name;
	// to_send += RPL_NOTOPIC;
	_messages.push_back(message(to_send, fd));
	to_send = ":"+_name+" 353 = ";
	to_send += get_client_by_fd(fd).get_nickname() + " ";
	to_send += chan_name;
	to_send += " :@";
	to_send += get_client_by_fd(fd).get_nickname();
	to_send += "\r\n";
	_messages.push_back(message(to_send, fd));
	to_send = ":"+_name+" 366 ";
	to_send += get_client_by_fd(fd).get_nickname() + " ";
	to_send += chan_name;
	to_send += RPL_ENDOFNAMES;
	_messages.push_back(message(to_send, fd));
}

static int	isspecial(char c){
	if (c == '[' || c == ']' || c == '{' || c == '}' ||  c == '\\' ||  c == '|' ||  c == '`' ||  c == ',' || c == '^' || c == '_')
		return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

int	server::handle_errone(commande &param){
	std::string	to_send;

	std::cout << param.get_params() << std::endl;
	if (param.get_params().size() > 9){
		to_send = "432 ";
		to_send += param.get_params();
		to_send += ERR_ERRONEUSNICKNAME;
		_messages.push_back(message(to_send, param.get_fd()));
		return (EXIT_FAILURE);
	}
	if (isspecial(param.get_params().at(0)) == EXIT_FAILURE && !isalpha(param.get_params().at(0))){
		to_send = "432 ";
		to_send += param.get_params();
		to_send += ERR_ERRONEUSNICKNAME;
		_messages.push_back(message(to_send, param.get_fd()));
		return (EXIT_FAILURE);
	}
	for (size_t i = 1; i < param.get_params().size(); i++){
		if (isspecial(param.get_params().at(i)) == EXIT_FAILURE && !isalpha(param.get_params().at(i)) && !isdigit(param.get_params().at(i)) && (param.get_params().at(i)) != '-'){
			to_send = "432 ";
			to_send += param.get_params();
			to_send += ERR_ERRONEUSNICKNAME;
			_messages.push_back(message(to_send, param.get_fd()));
			return (EXIT_FAILURE);
		}
	}
	return (EXIT_SUCCESS);
}

// void	server::remove_all_channels_of_user(client &user){
// 	const std::string channel_list = user.channel_name_list();

// 	cmd_part(commande(channel_list, user.get_socket(), "PART"));
// }

// void	server::remove_one_channel_of_user(client &user, std::string which_chan){
// 	cmd_part(commande(which_chan, user.get_socket(), "PART"));
// }