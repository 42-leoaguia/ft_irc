#include "Mode.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Replies.hpp"

Mode::Mode()
{
}

Mode::~Mode()
{
}

void Mode::execute(Server &server,
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
			+ " MODE :"
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

	/*
	** MODE #channel
	*/
	if (params.size() == 1)
	{
		client.sendMessage(
			":ircserv 324 "
			+ client.getNickname()
			+ " "
			+ channelName
			+ " "
			+ channel->getModeString()
			+ "\r\n");

		return;
	}

	/*
	** Only operators may change modes
	*/
	if (!channel->isOperator(client))
	{
		client.sendMessage(
			std::string(":ircserv ")
			+ ERR_CHANOPRIVSNEEDED
			+ " "
			+ client.getNickname()
			+ " "
			+ channelName
			+ " :"
			+ ERR_CHANOPRIVSNEEDED_TEXT
			+ "\r\n");

		return;
	}

	std::string modeString = params[1];

	bool adding = true;

	for (size_t i = 0; i < modeString.size(); ++i)
	{
		char mode = modeString[i];

		if (mode == '+')
		{
			adding = true;
			continue;
		}

		if (mode == '-')
		{
			adding = false;
			continue;
		}

		if (mode == 'i')
			channel->setInviteOnly(adding);
		else if (mode == 't')
			channel->setTopicRestricted(adding);
		else
		{
			client.sendMessage(
				std::string(":ircserv ")
				+ ERR_UNKNOWNMODE
				+ " "
				+ client.getNickname()
				+ " "
				+ mode
				+ " :"
				+ ERR_UNKNOWNMODE_TEXT
				+ "\r\n");

			continue;
		}
	}

	std::string msg =
		":" + client.getNickname()
		+ "!"
		+ client.getUsername()
		+ "@"
		+ client.getHost()
		+ " MODE "
		+ channelName
		+ " "
		+ modeString
		+ "\r\n";

	channel->broadcast(msg, NULL);
}