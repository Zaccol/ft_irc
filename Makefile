NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

DEBUG =
# ifdef DEBUG
# CXXFLAGS += -fsanitize=address -fsanitize=undefined -fsanitize=leak -g3
# endif

OBJ_DIR = objs
SRC_DIR = $(shell find srcs -type d)
INC_DIR = ./inc

IFLAGS = -I$(INC_DIR)

vpath %.cpp $(foreach dir, $(SRC_DIR), $(dir):)

CLASS = server.cpp client.cpp channel.cpp commande.cpp message.cpp
# TMP_CMDS = nick.cpp pass.cpp
CHAN_CMDS = invite.cpp join.cpp kick.cpp part.cpp topic.cpp mode.cpp
MSG_CMDS = notice.cpp privmsg.cpp
USER_CMDS = nick.cpp pass.cpp user.cpp cap.cpp quit.cpp
USER_QUERIES = who.cpp
SERVER = server_loop.cpp

SRC = main.cpp $(CLASS) $(SERVER) $(CHAN_CMDS) $(MSG_CMDS) $(USER_CMDS) $(USER_QUERIES)

OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))

all: test $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c $< -o $@

test:
	@mkdir -p objs
	@mkdir -p objs/./srcs/commands/commands_test/

clean:
	rm -rf $(OBJ_DIR)

re: clean all

fclean: clean
	rm -f $(NAME)

.PHONY: all clean re fclean