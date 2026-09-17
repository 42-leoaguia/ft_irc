#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>

class Client;

class Channel
{
private:
	std::string _name;
	std::string _topic;

	std::vector<Client*> _members;
	std::vector<Client*> _operators;

public:
	Channel();
	Channel(const std::string& name);
	~Channel();

	// Channel info
	const std::string& getName() const;
	const std::string& getTopic() const;
	void setTopic(const std::string& topic);

	// Members
	void addMember(Client* client);
	void removeMember(Client* client);
	bool isMember(Client* client) const;

	// Operators
	void addOperator(Client* client);
	void removeOperator(Client* client);
	bool isOperator(Client* client) const;

	// Messaging
	void broadcast(const std::string& msg, Client* except);

	// Utilities
	std::string getNamesList() const;

	const std::vector<Client*>& getMembers() const;
	const std::vector<Client*>& getOperators() const;
};

#endif