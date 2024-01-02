#include "../../include/Commands.hpp"

void PrivMsg::privMsg( Client* client, vector<string> commandParts, Server* srv )
{
	if (commandParts.size() < 2) {
		client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "PRIVMSG"));
		return;
	}

	string commandString = mergeString(commandParts, " ");
	size_t targetStart = commandString.find("PRIVMSG") + 8;
	size_t messageStart = commandString.find(" :", targetStart);
	if (messageStart == string::npos) {
		client->sendReply(ERR_NOTEXTTOSEND(client->getNickName()));
		return;
	}
	string target = commandString.substr(targetStart, messageStart - targetStart);
	target = strim(target);
	string message = commandString.substr(messageStart + 2);

	if (target.at(0) == '#') {
		sendChannelMessage(client, target, message, srv);
	}
	else {
		sendPrivateMessage(client, target, message, srv);
	}
}
void sendChannelMessage( Client* client, string channelName, string message, Server* srv )
{	
	Channel* channel = srv->getChannel(channelName);
	if (channel == NULL) {
		client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
		return;
	}
	if (channel->getModerated() && !client->isOperator())
	{
		client->sendReply(ERR_NOCANNOTSENDTOCHAN(client->getNickName(), channelName));
		return;
	}
	if (channel->getNoExternalMessages() && !channel->isUserOnChannel(client)) {
		client->sendReply(ERR_NOCANNOTSENDTOCHAN(client->getNickName(), channelName));
		return;
	}
	channel->broadcastMessage(":" + client->getPrefix() + " PRIVMSG " + channelName + " :" + message, client);
}

void sendPrivateMessage( Client* client, string target, string message, Server* srv )
{
	Client* targetClient = srv->getClient(target);
	if (targetClient == NULL) {
		client->sendReply(ERR_NOSUCHNICK(client->getNickName(), target));
		return;
	}
	targetClient->sendMessage(":" + client->getPrefix() + " PRIVMSG " + target + " :" + message);
}
