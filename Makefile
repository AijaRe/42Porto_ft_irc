# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/26 19:53:42 by arepsa            #+#    #+#              #
#    Updated: 2025/02/10 20:31:19 by arepsa           ###   ########.fr        #
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
OBJ_DIR		= objs
FILES		= main utils Server Client CommandHandler Channel Replies
SRC			= $(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJ			= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(FILES)))
DEPS		= $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	echo "\033[32mDone compiling $(NAME)!\033[0m"

# the - prefix in the include directive prevents make from quitting 
# if an included file doesn't exist
-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir $(OBJ_DIR)
	
clean:
	@$(RM) $(OBJ_DIR)
	echo "\033[31mDeleted object files and dependencies\033[0m"

fclean: clean
	@$(RM) $(NAME)
	echo "\033[31mDeleted $(NAME)\033[0m"

re: fclean all

.PHONY : all clean fclean re
.SILENT:
