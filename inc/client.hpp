#pragma once

#include "irc.hpp"

class channel;
class commande;

class client{
	private:
		std::string				_nickname;
		std::string				_username;
		std::string				_realname;
		std::string				_hostname;
		int						_socket;
		std::vector<channel>	_channel_list;
		bool					_userdone;
		bool					_passdone;
		bool					_nickdone;


	public:
		std::string				buf;
		client(int socket);
		client(std::string name, int socket);
		~client();

		const std::string	&get_nickname(void);
		const std::string	&get_username(void);
		const std::string	&get_realname(void);
		const std::string	&get_hostname(void);
		const bool			&get_userdone(void);
		const bool			&get_passdone(void);
		const bool			&get_nickdone(void);
		const int			&get_socket(void);

		void				set_nickname(std::string name);
		void				set_username(std::string username);
		void				set_realname(std::string realname);
		void				set_hostname(std::string hostname);
		void				set_userdone_true();
		void				set_passdone_true();
		void				set_nickdone_true();
		void				set_userdone_false();
		void				set_passdone_false();
		void				set_nickdone_false();

		int					is_it_tonchan(std::string which_chan);
		void				add_chan_to_client(channel &chan);
		std::string			part_list_channels_of_user();
		void				remove_all_channels_of_user();
		void				remove_one_channel_of_user(std::string which_chan);
		channel				&get_channel(std::string chan_name);
		std::string			channel_name_list();
		std::string			get_last_channel();
		int					get_nb_channels();
};