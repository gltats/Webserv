# Banner
define T_HEADER

██╗    ██╗███████╗██████╗     ███████╗███████╗██████╗ ██╗   ██╗
██║    ██║██╔════╝██╔══██╗    ██╔════╝██╔════╝██╔══██╗██║   ██║
██║ █╗ ██║█████╗  ██████╔╝    ███████╗█████╗  ██████╔╝██║   ██║
██║███╗██║██╔══╝  ██╔══██╗    ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝
╚███╔███╔╝███████╗██████╔╝    ███████║███████╗██║  ██║ ╚████╔╝
 ╚══╝╚══╝ ╚══════╝╚═════╝     ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝

endef
export T_HEADER

#Colors------------------------------------------------------------
#\033[38;2;255;0;0m 38;2 indicates 24-bit color mode.
#                   255;0;0 amount of red, green, and blue
CYAN = \033[0;96m
ORANGE = \033[38;2;255;146;3m
RED = \033[38;2;255;0;0m
GREEN = \033[38;2;0;255;0m
RESET = \033[0m
#------------------------------------------------------------------

#Emojis-------------------------
EMOJI_HAPPY := \xF0\x9F\x98\x83
EMOJI_SAD := \xF0\x9F\x98\xA2
EMOJI_CELEBRATE := \xF0\x9F\x8E\x89
EMOJI_CLOCK := \xE2\x8F\xB0
CLEANING_TOOL := \xF0\x9F\xA7\xBD
#--------------------------------

# Binary name
NAME = webserv

# Compiler and flags
# CC = clang++ // incorrect compiler
CC = c++
#CFLAGS = -Wall -Wextra -Werror -Iincludes -std=c++98
CFLAGS = -Iincludes -std=c++98  # in mac for development
# CFLAGS =  -Iincludes -std=c++98 -D__linux__  # for development in docker linux

# Folders & files
# FILES = main ConfigParser # Tatiana's original
FILES = main_server map signal_handler lib_convertion ConfigParser Server Request Response Connection
HEADER_FILES = webserver

# Source files
SRC = $(addsuffix .cpp, $(addprefix src/, $(FILES)))
HEADER = $(addsuffix .hpp, $(addprefix -Iincludes/, $(HEADER_FILES)))

# Object files
OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(CYAN)$$T_HEADER$(RESET)"
	@echo "\n"
	@echo "$(EMOJI_CLOCK)$(ORANGE)Compiling...$(RESET)"
	@$(CC) $(CFLAGS) $(SRC) -o $(NAME)
	@echo "$(EMOJI_HAPPY)$(GREEN)DONE ✓✓$(RESET)"

%.o: %.cpp
	@echo "$(ORANGE)Generating objects... $@$(RESET)"
	@${CC} ${CFLAGS} -c $< -o $@

clean:
	@echo "$(EMOJI_CLOCK)$(ORANGE)cleaning...$(RESET)"
	@rm -f $(OBJ)
	@echo "$(EMOJI_HAPPY)$(GREEN)DONE ✓✓$(RESET)"

fclean: clean
	@echo "$(EMOJI_CLOCK)$(ORANGE)cleaning...$(RESET)"
	@rm -f $(NAME)
	@echo "$(EMOJI_HAPPY)$(GREEN)DONE ✓✓$(RESET)"

re: fclean all

.PHONY: clean fclean re