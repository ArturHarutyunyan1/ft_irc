NAME = ircserv
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g
SRC = $(wildcard ./src/*.cpp) $(wildcard ./src/commands/*.cpp)
INC = $(wildcard ./include/*.hpp)
OBJ = ${SRC:.cpp=.o}
CPP = c++
RM = rm -f

$(NAME): $(OBJ) $(INC)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ)
%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@
all: $(NAME)
clean:
	$(RM) $(OBJ)
fclean: clean
	$(RM) $(NAME)
re: fclean all
.PHONY: all re fclean clean
