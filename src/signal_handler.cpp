#include "webserver.hpp"

void	ft_signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << CYAN << std::endl << "User request to close program with ctrl+c. Exiting..." << RESET << std::endl;
		// throw exception and the moment exiting directly
		exit(0);
	}
}

void	ft_setup_sighandler(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sa.sa_flags = 0;
	sa.sa_handler = &ft_signal_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << REDB << "Error: ft_setup_sighandler" << RESET << std::endl;
		// throw exception or at the moment exit wit error
		exit(1);
	}
}