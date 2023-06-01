#include "irc.hpp"

bool is_running = true;

static void signal_handler(int signal) {
    if (signal == SIGINT) {
        std::cout << "\n\x1b[1;5;31mServer socket closed\x1b[0m" << std::endl;
        is_running = false;
    }
}

void	server_loop(server &serv)
{
	while (is_running)
	{
		signal(SIGINT, &signal_handler);
		signal(SIGQUIT, SIG_IGN);
		fd_set	read_fds = serv.ret_read_fds();
		fd_set	write_fds = serv.ret_write_fds();

		// Attente de l'activité sur le(s) fds clients
		if (select(serv.get_max_fd() + 1, &read_fds, &write_fds, NULL, NULL) < 0)
		{
			// std::cerr << "Error: select() could not read fds\n";
			return ;
		}
		// Nouvelle connexion de client
		if (FD_ISSET(serv.get_server_fd(), &read_fds))
			if (serv.add_client() == 1)
				return ;
		serv.send_messages(write_fds);
		// Lecture des messages envoyés par les clients
		serv.fct_buffer(read_fds);
	}
}
