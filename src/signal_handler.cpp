#include "webserver.hpp"
#include "Webserv_Exceptions.hpp"

void	ft_signal_handler(int signum)
{
	if (signum == SIGINT)
		throw UserRequestTermination();
}

void	ft_setup_sighandler(void)
{
	struct sigaction	sa;

	clear_memory(&sa, sizeof(sa));
	signal(SIGPIPE, SIG_IGN);
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sa.sa_flags = 0;
	sa.sa_handler = &ft_signal_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << REDB << "Error: could not set sigaction catcher for SIGINT" << RESET << std::endl;
		exit(1);
	}
}