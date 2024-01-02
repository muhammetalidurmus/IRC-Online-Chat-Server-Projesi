#include "../../include/Commands.hpp"	// Path: include/Commands.hpp

void Kick::kick( Client* client, vector<string> commandParts, Server* srv )
{
	if ( commandParts.size() < 3 ){
		client->sendReply( ERR_NEEDMOREPARAMS( client->getNickName(), "KICK" ) );
		return ;
	}

	string channelNamee = commandParts.at(1);
	string nickName = commandParts.at(2);
	string reason = (commandParts.size() > 3) ? commandParts.at(3) : "No reason specified";

	if ( channelNamee.at(0) != '#' ){
		client->sendReply( ERR_NOSUCHCHANNEL( client->getNickName(), channelNamee ) );
		return;
	}

	Channel* channel = srv->getChannel( channelNamee );
	if ( !channel ){
		client->sendReply( ERR_NOSUCHCHANNEL( client->getNickName(), channelNamee ) );
		return;
	}
	if ( !channel->isUserOnChannel( client ) ){
		client->sendReply( ERR_NOTONCHANNEL( client->getNickName(), channelNamee ) );
		return;
	}

	Client* target = srv->getClient( nickName );
	if ( !target ){
		client->sendReply( ERR_NOSUCHNICK( client->getNickName(), nickName ) );
		return;
	}
	if ( !channel->isUserOnChannel( target ) ){
		client->sendReply( ERR_USERNOTINCHANNEL( client->getNickName(), nickName, channelNamee ) );
		return;
	}
	string channelName = atrim(channelNamee);

	if (channel->getChannelOwner()->getNickName() == target->getNickName()){
			client->sendReply( ERR_NOKICKCHANNELOWNER(client->getNickName(), channelName));
		return;
	}
	if (client->isOperator()){
		channel->removeUserFromChannel( target );
		target->removeChannel( channel );
		string message = ":" + client->getPrefix() + " KICK " + channelName + " " + nickName + " :" + reason + "\n";
		channel->broadcastMessage( message, client );
		target->sendMessage( ":" + client->getPrefix() + " KICK " + channelName + " " + nickName + " :" + reason );
		target->sendMessage( "You have been kicked from channel " + channelName + " by " + client->getNickName() );
		string leavemessage = client->getNickName() + " " + " he kicked " + target->getNickName() + " from channel " + channel->getChannelName();
		log(leavemessage);
	}
	else if (!client->isOperator())
	{
		client->sendReply( ERR_CHANOPRIVSNEEDED( client->getNickName(), channelName ) );
		return;
	}
	if ( channel->getChannelClientCount() == 0 ){
		string channelName = channel->getChannelName();
		srv->removeChannel( channelName );
	}
}
