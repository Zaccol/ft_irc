#pragma once

#include "irc.hpp"

class client;
class message;

class channel{
	private:
		std::string 			_name;
		std::string 			_topic;
		std::vector<client>		user_list;
		std::vector<client>		op_user_list;
		bool					_invite_only;
		int						_limit_users;
		bool					_topic_restrict;
		std::string				_password;
	public:
		channel(std::string name);
		~channel();

		size_t						get_nb_users(void);
		int							&get_limit_users(void);
		void						set_limit_users(int nb);
		const std::string			&get_name(void);
		void						set_name(std::string name);
		const std::string			&get_topic(void);
		void						set_topic(std::string topic);
		const bool					&get_invite_only(void);
		void						set_invite_only_true();
		void						set_invite_only_false();
		const bool					&get_topic_restrict(void);
		void						set_topic_restrict_true();
		void						set_topic_restrict_false();
		const std::string			&get_password(void);
		void						set_password(std::string password);

		int							is_client_in_chan(std::string which_client);
		int							is_client_operator(std::string which_client);
		void						add_client_to_chan(client &user);
		void						add_op_client_to_chan(client &user);
		std::string					list_of_clients_nickname(void);
		std::vector<message>		send_message_to_all_users_of_this_channel(std::string msg, std::string not_this_client);
		void						remove_client_from_chan(client &user);
		void						remove_privilege_from_chan(client &user);
		std::vector<std::string>	send_rpl_whoreply_with_mode(std::string _name, std::string chan_name);
		std::vector<std::string>	send_rpl_whoreply_without_mode(std::string _name, std::string chan_name);
		std::vector<message>		warning_users_topic_changed(std::string who_did, std::string server_name, std::string new_topic);
		std::vector<message>		send_part_msg_to_everyone(std::string msg);
		std::vector<message>		send_join_msg_to_everyone(std::string msg);
		int							is_chan_empty(void);
		void						remove_password(void);
};