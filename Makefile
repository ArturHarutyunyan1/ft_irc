NAME = ircserv
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -I./opssl/include -g
LDFLAGS = -L./opssl/lib -lssl -lcrypto
SRC = $(wildcard ./src/*.cpp) $(wildcard ./src/commands/*.cpp)
INC = $(wildcard ./include/*.hpp )
OBJ = ${SRC:.cpp=.o}
LIBSSL_PATH = opssl
LIBSSL = $(LIBSSL_PATH)/lib/libssl.a
LIBCRYPTO = $(LIBSSL_PATH)/lib/libcrypto.a
OPENSSL_INC = $(wildcard $(LIBSSL_PATH)/include/openssl/*.h)
CPP = c++
RM = rm -rf

$(NAME): $(LIBSSL) $(LIBCRYPTO) $(OBJ) $(INC) $(OPENSSL_INC) Makefile
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJ) $(LDFLAGS)
%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@
all: $(NAME)
clean:
	$(RM) $(OBJ) opssl
fclean: clean
	$(RM) $(NAME)

re: fclean all
.PHONY: all re fclean clean

$(LIBSSL): $(OPENSSL_INC)
	tar -xvf openssl.tar

$(OPENSSL_INC):

$(LIBCRYPTO): $(OPENSSL_INC)