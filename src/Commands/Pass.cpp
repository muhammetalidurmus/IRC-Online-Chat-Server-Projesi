#include "../../include/Commands.hpp"

void Pass::pass( Client* client, const vector<string> commandParts, Server* srv)
{
	if (client->isRegistered() || client->getUserAuth() || client->getIsPass()){
		client->sendReply(ERR_ALREADYREGISTERED(client->getNickName()));
		return ;
	}
	if (commandParts.size() < 2) {
		client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "PASS"));
		return;
	}

	string passw = commandParts.at(1);
	if (!srv->verifySrvPass(passw)){
		client->sendMessage("Access denied!");
		return;
	}
	client->setPass(true);
	client->sendMessage("Password accepted!");
}
