/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liafonse <liafonse@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 12:40:45 by liafonse          #+#    #+#             */
/*   Updated: 2026/10/28 12:48:03 by liafonse         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Topic.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"

Topic::Topic()
{
}

Topic::~Topic()
{
}

void Topic::execute(Server &server,
					Client &client,
					const std::vector<std::string> &params)
{
	if (params.empty())
		return;

	std::string channelName = params[0];

	Channel *channel = server.getChannel(channelName);

	if (!channel)
	{
		client.sendMessage(
			std::string(":ircserv ")
			+ ERR_NOSUCHCHANNEL
			+ " "
			+ client.getNickname()
			+ " "
			+ channelName
			+ " :"
			+ ERR_NOSUCHCHANNEL_TEXT
			+ "\r\n");
		return;
	}

	if (!channel->isMember(client))
	{
		client.sendMessage(
			std::string(":ircserv ")
			+ ERR_NOTONCHANNEL
			+ " "
			+ client.getNickname()
			+ " "
			+ channelName
			+ " :"
			+ ERR_NOTONCHANNEL_TEXT
			+ "\r\n");
		return;
	}

	/*
	** TOPIC #general
	*/
	if (params.size() == 1)
	{
		if (channel->getTopic().empty())
		{
			client.sendMessage(
				std::string(":ircserv ")
				+ RPL_NOTOPIC
				+ " "
				+ client.getNickname()
				+ " "
				+ channelName
				+ " :"
				+ RPL_NOTOPIC_TEXT
				+ "\r\n");
		}
		else
		{
			client.sendMessage(
				std::string(":ircserv ")
				+ RPL_TOPIC
				+ " "
				+ client.getNickname()
				+ " "
				+ channelName
				+ " :"
				+ channel->getTopic()
				+ "\r\n");
		}

		return;
	}

	/*
	** TOPIC #general :new topic
	*/
	std::string newTopic = params[1];

	channel->setTopic(newTopic);

	std::string msg =
		":" + client.getNickname()
		+ "!"
		+ client.getUsername()
		+ "@"
		+ client.getHost()
		+ " TOPIC "
		+ channelName
		+ " :"
		+ newTopic
		+ "\r\n";

	channel->broadcast(msg, NULL);
}