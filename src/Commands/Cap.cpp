#include "../../include/Commands.hpp"

void Cap::cap( Client* client, const vector<string> commandParts)
{
	if (commandParts.size() < 2) {
		client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "CAP"));
		return;
	}
	string cap = commandParts.at(1);
	if (cap == "LS" || cap== "ls"){
		client->sendMessage("/PASS\n/USER\n/NICK\n/JOIN\n/PART\n/PRIVMSG\n/NOTICE\n/KICK\n/MODE\n/QUIT\n");
	}
	else {
		client->sendReply(ERR_INVALIDCAPCMD(client->getNickName()));
	}
}
