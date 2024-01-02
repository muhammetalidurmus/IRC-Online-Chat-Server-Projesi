#include "../../include/Commands.hpp"

void Mode::mode( Client* client, vector<string> commandParts, Server* srv )
{
	if (commandParts.size() < 2)
	{
		client->sendMessage( ":" + client->getHostName() + " 461 " + client->getNickName() + " MODE :Not enough parameters" );
		return ;
	}

	string atargetE = commandParts.at(1);
	string mode = (commandParts.size() > 2) ? commandParts.at(2) : "";
	string modeParams = "100";

	if (atargetE.empty())
	{
		client->sendMessage( ":" + client->getHostName() + " 403 " + client->getNickName() + " " + atargetE + " :No such channel\r\n" );
		return ;
	}
	else

	if (atargetE.at(0) == '#')
	{
		string atarget = atrim(atargetE);
		Channel* channel = srv->getChannel(atarget);
		if (!channel){
			client->sendMessage( ":" + client->getHostName() + " 403 " + client->getNickName() + " " + atargetE + " :No such channel\r\n" );
			return ;
		}
		if (mode.empty())
			return ;
		if (mode.at(0) == '+'){
			if (commandParts.size() > 3){
				modeParams = commandParts.at(3);
				channel->setUpModeChannel(channel, client, mode, modeParams);
			}
			else
				channel->setUpModeChannel(channel, client, mode, modeParams);
		}
		else if (mode.at(0) == '-')
			if (commandParts.size() > 3){
				modeParams = commandParts.at(3);
				channel->setLowModeChannel(channel, client, mode, modeParams);
			}
			else
				channel->setLowModeChannel(channel, client, mode, modeParams);
		else
			client->sendMessage( ":" + client->getHostName() + " 501 " + client->getNickName() + " :Unknown MODE flag" );
	}
	else
		client->sendMessage( ":" + client->getHostName() + " 501 " + client->getNickName() + " :Unknown MODE flag" );

}
