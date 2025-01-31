# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yfontene <yfontene@student.42porto>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/26 19:53:42 by arepsa            #+#    #+#              #
#    Updated: 2025/01/31 18:46:18 by yfontene         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++
RM = rm -rf

CXXFLAGS 	= -Wall -Werror -Wextra -pedantic-errors -fsanitize=address

#generate dependency files
CPPFLAGS	= -MMD

NAME		= ircserv

SRC_DIR		= srcs
INC_DIR		= incs
FILES		= main utils Server CommandHandler User
SRC			= $(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJ			= $(addprefix $(SRC_DIR)/, $(addsuffix .o, $(FILES)))
DEPS		= $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	echo "\033[32mDone compiling $(NAME)!\033[0m"

# the - prefix in the include directive prevents make from quitting 
# if an included file doesn't exist
-include $(DEPS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@
	
clean:
	@$(RM) $(OBJ) $(DEPS)
	echo "\033[31mDeleted object files and dependencies\033[0m"

fclean: clean
	@$(RM) $(NAME)
	echo "\033[31mDeleted $(NAME)\033[0m"

re: fclean all

.PHONY : all clean fclean re
.SILENT:
