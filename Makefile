# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/26 19:53:42 by arepsa            #+#    #+#              #
#    Updated: 2025/02/23 20:54:08 by arepsa           ###   ########.fr        #
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
MAIN_FILES	= main utils Client CommandHandler Replies
CMD_FILES	= handleCap handleJoin handleKick handleModes handleNick handleNotice \
			  handlePass handlePing handlePrivmsg handleUsers handleTopic handleInvite \
			  handleWho handleTests
CH_FILES	= ChannelBasics ChannelMembers ChannelSettings
SRV_FILES	= ServerBasics ServerClients ServerChannels ServerConnections
SRC			= $(addprefix $(SRC_DIR)/, $(addsuffix .cpp, $(MAIN_FILES))) \
			  $(addprefix $(SRC_DIR)/commands/, $(addsuffix .cpp, $(CMD_FILES))) \
			  $(addprefix $(SRC_DIR)/channel/, $(addsuffix .cpp, $(CH_FILES))) \
			  $(addprefix $(SRC_DIR)/server/, $(addsuffix .cpp, $(SRV_FILES)))
OBJ			= $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(MAIN_FILES))) \
			  $(addprefix $(OBJ_DIR)/commands/, $(addsuffix .o, $(CMD_FILES))) \
			  $(addprefix $(OBJ_DIR)/channel/, $(addsuffix .o, $(CH_FILES))) \
			  $(addprefix $(OBJ_DIR)/server/, $(addsuffix .o, $(SRV_FILES)))
DEPS		= $(OBJ:.o=.d)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	echo "\033[32mDone compiling $(NAME)!\033[0m"

# the - prefix in the include directive prevents make from quitting 
# if an included file doesn't exist
-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR)/commands/%.o: $(SRC_DIR)/commands/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR)/channel/%.o: $(SRC_DIR)/channel/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR)/server/%.o: $(SRC_DIR)/server/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/commands
	@mkdir -p $(OBJ_DIR)/channel
	@mkdir -p $(OBJ_DIR)/server
	
clean:
	@$(RM) $(OBJ_DIR)
	echo "\033[31mDeleted object files and dependencies\033[0m"

fclean: clean
	@$(RM) $(NAME)
	echo "\033[31mDeleted $(NAME)\033[0m"

re: fclean all

.PHONY : all clean fclean re
.SILENT:
