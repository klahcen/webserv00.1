CXX = c++

CXXFLAGS =  -Wall -Wextra -Werror -std=c++98 

NAME = webserv

SRCS = main.cpp src/CGIHandler.cpp  src/ConfigParser.cpp   src/HttpRequest.cpp  src/HttpResponse.cpp src/MethodDelete.cpp src/MethodGet.cpp  src/MethodPost.cpp  src/RouteConfig.cpp  src/ServerConfig.cpp  src/Server.cpp


OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
		$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all 

.PHONEY: all clean fclean re