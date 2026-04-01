NAME	:=	ircserver

CXX		:=	c++
CXFLAGS	:=	-Wall -Wextra -Werror -g

DIR_SRC	:=	src
DIR_OBJ	:=	obj
INCLUDE	:=	-Iincludes

SRC		:=	main.cpp Server.cpp Client.cpp Channel.cpp
OBJ		:=	$(SRC:.cpp=.o)
OBJ		:=	$(addprefix $(DIR_OBJ)/, $(OBJ))

all		:	$(NAME)

$(NAME)	:	$(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $@
	@echo "✅ Compilation completed: $(NAME)"

$(DIR_OBJ)/%.o	:	$(DIR_SRC)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

clean	:
	@rm -rf $(DIR_OBJ)
	@echo "✅ clean done."

fclean	:	clean
	@rm -f $(NAME)
	@echo "✅ fclean done."

re		:	fclean all

.PHONY	:	all clean fclean re