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
	public:
		channel(std::string name);
		~channel();

		const std::string			&get_name(void);
		void						set_name(std::string name);

		const std::string			&get_topic(void);
		void						set_topic(std::string topic);

		int							is_client_in_chan(std::string which_client);
		int							is_client_operator(std::string which_client);
		void						add_client_to_chan(client &user);
		void						add_op_client_to_chan(client &user);
		std::string					list_of_clients_nickname(void);
		std::vector<message>		send_message_to_all_users_of_this_channel(std::string msg, std::string not_this_client);
		void						remove_client_from_chan(client &user);
		std::vector<std::string>	send_rpl_whoreply_with_mode(std::string _name, std::string chan_name);
		std::vector<std::string>	send_rpl_whoreply_without_mode(std::string _name, std::string chan_name);
		std::vector<message>		warning_users_topic_changed(std::string who_did, std::string server_name, std::string new_topic);
		std::vector<message>		send_part_msg_to_everyone(std::string msg);
		std::vector<message>		send_join_msg_to_everyone(std::string msg);
};