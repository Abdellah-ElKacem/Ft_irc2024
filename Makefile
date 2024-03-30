NAME = ircserv

HEADER = Server.hpp Clients.hpp 

SRC = main.cpp Server.cpp Clients.cpp identify_client/authenticate.cpp identify_client/register.cpp \
	identify_client/parce_cmd.cpp messages.cpp

OBJ = $(SRC:.cpp=.o)

CPPFLAGS = -Wall -Wextra -Werror -std=c++98

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