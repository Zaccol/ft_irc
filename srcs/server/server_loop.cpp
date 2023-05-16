#include "irc.hpp"

static void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n\x1b[1;31mServer socket closed\x1b[0m" << std::endl;
        exit(128 + signal);
    }
}

int	server_loop(server serv)
{
	// std::string	cmds_str[10] = {"INVITE", "JOIN", "KICK", "PART", "TOPIC", "PRIVMSG", "NOTICE", "NICK", "PASS", "USER"};

	while (true)
	{
		signal(SIGINT, &signal_handler);
		fd_set	read_fds = serv.ret_read_fds();
		fd_set	write_fds = serv.ret_write_fds();

		// Attente de l'activité sur le(s) fds clients
		if (select(serv.get_max_fd() + 1, &read_fds, &write_fds, NULL, NULL) < 0)
		{
			std::cerr << "Error: select() could not read fds\n";
			return (1);
		}
		// Nouvelle connexion de client
		if (FD_ISSET(serv.get_server_fd(), &read_fds))
			if (serv.add_client() == 1)
				return (1);
		serv.send_messages(write_fds);
		// std::cout << "ca passe" << std::endl;
		// Lecture des messages envoyés par les clients
		// read_msg_clients(serv, read_fds, write_fds);
		serv.fct_buffer(read_fds);
	}
}
