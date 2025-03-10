/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42porto.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/31 11:02:57 by yfontene          #+#    #+#             */
/*   Updated: 2025/03/10 22:18:25 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "utils.hpp"

class Channel;

class Client
{
	public:
		Client();
		Client(int fd, const std::string &host);
		~Client();
		
		//setters
		void		setNickname(const std::string &nickname);
		void		setUsername(const std::string &username);
		void		setRealname(const std::string &realname);
		void		setMode(char mode, bool enable);
		void		setPassAuthenticated(bool status);
		void		setWelcomed(bool welcomed);

		//getters
		int			getFd() const;
		std::string	getHost() const;
		std::string	getNickname() const;
		std::string	getUsername() const;
		const std::string&	getRealname() const;
		std::string	getModes() const;

		//helper functions
		bool		passAuthenticated() const;
		bool		isFullyAuthenticated() const;
		bool		isConnected() const;
		bool		isWelcomed() const;
		bool		hasMode(char mode) const;
		void		addChannel(Channel *channel);
		void		removeChannel(Channel *channel);
		bool		operator==(const Client &compare) const; //Used inside Channel::findUser()
		
		std::string	buffer;
		std::map<std::string, Channel*>	joinedChannels;
	
	private:
		int			_fd;
		std::string	_host;
		bool		_pass_authenticated;
		std::string	_nickname;
		std::string	_username;
		std::string	_realname;
		std::string	_modes;
		bool		_welcomed; // flag that indicates if the user has received connection messages
};

#endif
