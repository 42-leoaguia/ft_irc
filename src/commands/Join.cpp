#include "Join.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Replies.hpp"
#include <sstream>
#include <iostream>
#include <cstdio>

Join::Join()
{
}

Join::~Join()
{
}

static std::vector<std::string> splitChannels(const std::string &str)
{
	std::vector<std::string> channels;
	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, ','))
		channels.push_back(token);

	return channels;
}

static bool isValidChannelName(const std::string& name)
{
	if (name.empty() ||
		name[0] != '#' ||
		name.find(' ') != std::string::npos)
		return false;

	return true;
}

void Join::execute(Server &server,
				   Client &client,
				   const std::vector<std::string> &params)
{
	if (params.empty())
	{
		client.sendMessage(
			std::string(":ircserv ") +
			ERR_NEEDMOREPARAMS + " " +
			client.getNickname() +
			" JOIN :" +
			ERR_NEEDMOREPARAMS_TEXT +
			"\r\n");
		return;
	}

	std::vector<std::string> channels =
		splitChannels(params[0]);

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string channelName = channels[i];

		if (!isValidChannelName(channelName))
		{
			client.sendMessage(
				std::string(":ircserv ") +
				ERR_NOSUCHCHANNEL + " " +
				client.getNickname() + " " +
				channelName + " :" +
				ERR_NOSUCHCHANNEL_TEXT +
				"\r\n");

			continue;
		}

		Channel *channel = server.getChannel(channelName);

		if (!channel)
		{
			channel = createChannel(server, client, channelName);
		} else if (channel->isMember(client))
			continue;

		if (channel->isInviteOnly()){
			client.sendMessage(
				std::string(":ircserv ")
				+ ERR_INVITEONLYCHAN + " "
				+ client.getNickname() + " "
				+ channelName + " :"
				+ ERR_INVITEONLYCHAN_TEXT
				+ "\r\n");

			continue;
		}

		channel->addMember(client);

		notifyJoin(*channel, client);

		sendChannelInfo(*channel, client);
	}
};

Channel* createChannel(Server& server, Client& client, std::string channelName) {
	server.addChannel(channelName);
	Channel *channel = server.getChannel(channelName);
	channel->addMember(client);
	channel->addOperator(client);
	return channel;
}

void notifyJoin(Channel& channel, Client& client){
	std::string joinMsg =
			":" + client.getNickname() +
			"!" + client.getUsername() +
			"@" + client.getHost() + 
			" JOIN " +
			channel.getName() + "\r\n";

	channel.broadcast(joinMsg, NULL);
}

void sendChannelInfo(Channel& channel, Client& client){
		sendTopic(channel, client);
		sendMembersList(channel, client);
}

void sendTopic(Channel& channel, Client& client){
	if (channel.getTopic().empty())
	{
		client.sendMessage(
			std::string(":ircserv ") +
			RPL_NOTOPIC + " " +
			client.getNickname() + " " +
			channel.getName() + " :" +
			RPL_NOTOPIC_TEXT +
			"\r\n");
	}
	else
	{
		client.sendMessage(
			std::string(":ircserv ") +
			RPL_TOPIC + " " +
			client.getNickname() + " " +
			channel.getName() + " :" +
			channel.getTopic() +
			"\r\n");
	}
}

void sendMembersList(Channel& channel, Client& client){
	client.sendMessage(
			std::string(":ircserv ") +
			RPL_NAMREPLY + " " +
			client.getNickname() +
			" " +
			channel.getName() +
			" :" +
			channel.getNamesList() +
			"\r\n");

	client.sendMessage(
			std::string(":ircserv ") +
			RPL_ENDOFNAMES + " " +
			client.getNickname() +
			" " +
			channel.getName() +
			" :" +
			RPL_ENDOFNAMES_TEXT +
			"\r\n");
}