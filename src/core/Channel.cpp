#include "Channel.hpp"
#include "Client.hpp"

#include <algorithm>

Channel::Channel()
{
}

Channel::Channel(const std::string& name)
	: _name(name)
{
}

Channel::~Channel()
{
}

const std::string& Channel::getName() const
{
	return _name;
}

const std::string& Channel::getTopic() const
{
	return _topic;
}

void Channel::setTopic(const std::string& topic)
{
	_topic = topic;
}

bool Channel::isMember(Client* client) const
{
	return std::find(
		_members.begin(),
		_members.end(),
		client
	) != _members.end();
}

void Channel::addMember(Client* client)
{
	if (!client)
		return;

	if (!isMember(client))
		_members.push_back(client);
}

void Channel::removeMember(Client* client)
{
	_members.erase(
		std::remove(
			_members.begin(),
			_members.end(),
			client
		),
		_members.end()
	);

	removeOperator(client);
}

bool Channel::isOperator(Client* client) const
{
	return std::find(
		_operators.begin(),
		_operators.end(),
		client
	) != _operators.end();
}

void Channel::addOperator(Client* client)
{
	if (!client)
		return;

	if (!isMember(client))
		return;

	if (!isOperator(client))
		_operators.push_back(client);
}

void Channel::removeOperator(Client* client)
{
	_operators.erase(
		std::remove(
			_operators.begin(),
			_operators.end(),
			client
		),
		_operators.end()
	);
}

void Channel::broadcast(const std::string& msg, Client* except)
{
	for (std::vector<Client*>::iterator it = _members.begin();
		 it != _members.end();
		 ++it)
	{
		if (*it == except)
			continue;

		(*it)->sendMessage(msg);
	}
}

std::string Channel::getNamesList() const
{
	std::string names;

	for (size_t i = 0; i < _members.size(); ++i)
	{
		Client* client = _members[i];

		if (i > 0)
			names += " ";

		if (isOperator(client))
			names += "@";

		names += client->getNickname();
	}

	return names;
}

const std::vector<Client*>& Channel::getMembers() const
{
	return _members;
}

const std::vector<Client*>& Channel::getOperators() const
{
	return _operators;
}