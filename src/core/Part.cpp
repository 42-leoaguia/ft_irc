#include "Part.hpp"
#include "Replies.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <iostream>

Part::Part()
{
}

Part::~Part()
{
}

void Part::execute(Server &server,
				   Client &client,
				   const std::vector<std::string> &params)
{
	if (params.empty())
	{
		client.sendMessage(
			std::string(":ircserv ")
			+ ERR_NEEDMOREPARAMS
			+ " "
			+ client.getNickname()
			+ " PART :"
			+ ERR_NEEDMOREPARAMS_TEXT
			+ "\r\n");
		return;
	}

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

	std::string reason;

	if (params.size() > 1)
		reason = params[1];

	std::string msg =
		":" + client.getNickname()
		+ "!"
		+ client.getUsername()
		+ "@"
		+ client.getHost()
		+ " PART "
		+ channelName;

	if (!reason.empty())
		msg += " :" + reason;

	msg += "\r\n";

	// author included
	channel->broadcast(msg, NULL);

	channel->removeOperator(client);
	channel->removeMember(client);

	if (channel->empty())
		server.removeChannel(channelName);
}