NAME = ircserv

HEADER = Server.hpp Clients.hpp channel.hpp

SRC = main.cpp Server.cpp Clients.cpp channel.cpp join_mode.cpp commands.cpp

OBJ = $(SRC:.cpp=.o)

# CPPFLAGS = -Wall -Wextra -Werror -std=c++98

CPP = c++

all : $(NAME)

$(NAME) : $(OBJ)
	$(CPP) $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o:%.cpp $(HEADER)
			 $(CPP) $(CPPFLAGS) -c $< -o $@

clean :
	rm -f $(OBJ)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY = clean