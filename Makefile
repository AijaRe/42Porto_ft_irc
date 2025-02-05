# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/26 19:53:42 by arepsa            #+#    #+#              #
#    Updated: 2025/02/05 17:56:34 by arepsa           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CXX = c++
RM = rm -rf

CXXFLAGS 	= -Wall -Werror -Wextra -pedantic-errors -std=c++98 #-fsanitize=address 

#generate dependency files
CPPFLAGS	= -MMD

NAME		= ircserv

SRC_DIR		= srcs
INC_DIR		= incs
FILES		= main utils Server Client CommandHandler
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
