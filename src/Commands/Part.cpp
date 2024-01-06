#include "../../include/Commands.hpp"

void Part::part( Client* client, vector<string> commandParts, Server* srv )
{
    if ( commandParts.size() < 2 ){
        client->sendMessage( ":" + client->getHostName() + " 461 " + client->getNickName() + " PART :Not enough parameters" );
        return ;
    }
    string target = commandParts.at(1);
    if ( target.find(',') == string::npos )
        partChannel( client, target, srv );
    else
    {
        istringstream iss( target );
        string channelName;
        while ( getline( iss, channelName, ',' ) )
            partChannel( client, channelName, srv );
    }
}

void Part::partChannel( Client* client, string channelName, Server* srv )
{
	if (channelName.empty() || channelName.at(0) != '#'){
		client->sendMessage( ":" + client->getHostName() + " 403 " + client->getNickName() + " " + channelName + " :No such channel" );
		return;
	}

	Channel* channel = srv->getChannel( channelName );
	if (!channel){
		client->sendMessage( ":" + client->getHostName() + " 403 " + client->getNickName() + " " + channelName + " :No such channel" );
		return;
	}
	if (!channel->isUserOnChannel( client )){
		client->sendMessage( ":" + client->getHostName() + " 442 " + client->getNickName() + " " + channelName + " :You're not on that channel" );
		return;
	}

	channel->removeUserFromChannel( client );
	client->removeChannel( channel );

	string message	= ":" + client->getPrefix() + " PART " + channelName + "\n";
	channel->broadcastMessage( message, client );
	client->sendMessage( "You left the channel " + channelName );
	//std::string leavemessage = client->getNickName() + " " + " has left the channel " + channel->getChannelName();
	//log(leavemessage);

	if ( channel->getChannelClientCount() == 0 && srv->channelExists( channelName )){
		string channelName = channel->getChannelName();
		string message = "Channel " + channelName + " is empty, deleting.\n";
		write( 1, message.c_str(), message.length() );
		srv->removeChannel( channelName );
	}
}
