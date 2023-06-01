#pragma once

class	client;
class	channel;
class	commande;
class	message;
#include "irc.hpp"

class server{
	public:
		// typedef void	(server::*cmd)(commande *);
	private:
		std::string					_name;
		std::string					_pass;
		std::vector<client>			_full_client_list;
		std::vector<channel>		_full_channel_list;
		int							_port;
		std::vector<message>		_messages;



		struct sockaddr_in				_client_addr;
		struct sockaddr_in				_server_addr;
		int								_server_fd;

	public:
		server(int port, char *password);
		~server();



		int							add_client();
		void						del_client(int fd);
		int							get_max_fd();
		int							get_server_fd() const;
		fd_set						ret_read_fds();
		fd_set						ret_write_fds();

		const std::string			&get_name(void);
		const std::string			&get_pass(void);
		const int					&get_port(void);

		void						set_name(std::string name);
		void						set_pass(std::string pass);
		void						set_port(int port);


		void						cmd_invite(commande &param);
		void						cmd_join(commande &param);
		void						cmd_kick(commande &param);
		void						cmd_part(commande &param);
		void						cmd_topic(commande &param);
		void						cmd_notice(commande &param);
		void						cmd_privmsg(commande &param);
		void						cmd_nick(commande &param);
		void						cmd_pass(commande &param);
		void						cmd_user(commande &param);
		void						cmd_cap();
		void						cmd_mode(commande &param);
		void						cmd_who(commande &param);
		void						cmd_quit(commande &param);

		int							find_name_occurence(const std::string name, const int fd);
		client						&get_client_by_name(const std::string name);
		client						&get_client_by_fd(const int fd);
		channel						&get_channel_by_name(const std::string name);
		std::vector<std::string>	ft_split(std::string param, std::string delim);
		std::vector<std::string>	ft_split_once(std::string param, std::string delim);
		int							client_exists(const std::string name);
		int							channel_exists(const std::string name);
		void						add_chan_to_server(channel &chan);
		void						add_client_to_server(client &user);
		int							is_valid_chan_name(commande &param, std::string chan_check);
		void						create_a_new_channel(commande &param, std::string new_channel_name);
		void						delete_chan(std::string which_chan);
		void						delete_client(std::string which_client);
		void						sending_replies_for_new_channel(int fd, std::string chan_name);
		void						sending_replies_for_existing_channel(int fd, std::string chan_name);
		int							handle_errone(commande &param);
		void						send_messages(fd_set write_fds);
		void						fct_buffer(fd_set &read_fds);
		void						handle_message(std::string line, int fd);
		commande					parsing_commands(std::string msg, int fd);
		std::string					list_active_mods(channel &chan);

};

void	server_loop(server &serv);
