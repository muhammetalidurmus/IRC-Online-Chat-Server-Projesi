NAME = ircserv
CC = g++

FLAGS = -Wall -Wextra -Werror -std=c++98 -pedantic -g

SRCS = main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp src/SrvUtils.cpp src/ChUtils.cpp src/Commands/CommandParser.cpp \
       src/Commands/PrivMsg.cpp src/Commands/Quit.cpp src/Commands/User.cpp src/Commands/Join.cpp \
	   src/Commands/Part.cpp src/Commands/Mode.cpp src/Commands/Kick.cpp src/Commands/Notice.cpp \
	   src/Commands/Pass.cpp src/Commands/Nick.cpp src/Commands/Cap.cpp src/Bot.cpp \

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean all re
