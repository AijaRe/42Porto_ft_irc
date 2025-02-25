/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInvite.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arepsa <arepsa@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 16:17:50 by uviana-a          #+#    #+#             */
/*   Updated: 2025/02/24 19:06:52 by arepsa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CommandHandler.hpp"

bool CommandHandler::inviteValidation(Client &issuer, const std::string &nick_to_invite, const std::string &channel_key, const std::string &test_param)
{
    std::vector<std::string> params;
    if (nick_to_invite.empty() || channel_key.empty() || !test_param.empty())
    {
        params.push_back("INVITE");
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_NEEDMOREPARAMS, params);
        return false;
    }
    //Does this channel exists?
    std::map<std::string, Channel>::iterator it = this->_server.getChannels().find(channel_key);
    if (it == this->_server.getChannels().end() || this->_server.getChannels().size() == 0)
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_NOSUCHCHANNEL, params);
        return false;
    }
    //The issuer is in the channel?
    if (!it->second.findUserByName(it->second.getClients(), issuer.getNickname()))
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_USERNOTINCHANNEL, params);
        return false;
    }
    /*ONLY VALID AFTER THE MODE VERIFIED*/
    //Is the issuer an OP?
    /*
    if (!it->second.findUserByName(it->second.getOps(), issuer.getNickname()))
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, issuer.getNickname(), ERR_CHANOPRIVSNEEDED, params);
        return false;
    }*/

    //The target is in the channel?
    if (it->second.findUserByName(it->second.getClients(), nick_to_invite))
    {
        params.push_back(channel_key);
        this->_replies.sendIrcInfo(issuer.getFd(), this->_server, nick_to_invite, ERR_USERONCHANNEL, params);
        return false;
    }

    return true;
}

//INVITE nick #channel
void CommandHandler::handleInvite(Client &client, std::istringstream &iss)
{
    std::string nick_to_invite, channel_key, test_param;

    iss >> nick_to_invite;
    iss >> channel_key;
    iss >> test_param;

    //WAITING MODE IN PLACE.
    if (!inviteValidation(client, nick_to_invite, channel_key, test_param))
        return ;
    else
    {
        std::cout << "Nick: " << nick_to_invite << std::endl;
        std::cout << "Channel: " << channel_key << std::endl;
    }

}
