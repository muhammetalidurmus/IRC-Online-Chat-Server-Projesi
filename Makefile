NAME = ircserv
CC = g++
FLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCDIR = src
OBJDIR = obj

SRCS = main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp src/SrvUtils.cpp src/ChUtils.cpp src/Commands/CommandParser.cpp \
       src/Commands/PrivMsg.cpp src/Commands/Quit.cpp src/Commands/User.cpp src/Commands/Join.cpp \
	   src/Commands/Part.cpp src/Commands/Mode.cpp src/Commands/Kick.cpp src/Commands/Notice.cpp \
	   src/Commands/Pass.cpp src/Commands/Nick.cpp src/Commands/Cap.cpp src/Bot.cpp \

OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

all: $(OBJDIR) $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(FLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/Commands

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: clean fclean all re