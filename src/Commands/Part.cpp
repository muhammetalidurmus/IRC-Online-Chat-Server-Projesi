#include "../../include/Commands.hpp"

void Part::part(Client* client, vector<string> commandParts, Server* srv)
{
    // Eğer komut parametreleri yetersizse
    if (commandParts.size() < 2)
    {
        client->sendMessage( ":" + client->getHostName() + " 461 " + client->getNickName() + " PART :Not enough parameters" );
        return;
    }

    // Hedef kanal adını al
    string target = commandParts.at(1);

    // Eğer hedefte virgül yoksa
    if (target.find(',') == string::npos)
        partChannel(client, target, srv);
    else
    {
        // Virgülle ayrılmış kanal adlarını işle
        istringstream iss(target);
        string channelName;
        while (getline(iss, channelName, ','))
            partChannel(client, channelName, srv);
    }
}

void Part::partChannel(Client* client, string channelName, Server* srv)
{
    // Eğer kanal adı boşsa veya '#' ile başlamıyorsa
    if (channelName.empty() || channelName.at(0) != '#')
    {
      	client->sendMessage( ":" + client->getHostName() + " 403 " + client->getNickName() + " " + channelName + " :No such channel" );
        return;
    }

    // Kanal nesnesini al
    Channel* channel = srv->getChannel(channelName);

    // Eğer kanal bulunamazsa
    if (!channel)
    {
     	client->sendMessage( ":" + client->getHostName() + " 403 " + client->getNickName() + " " + channelName + " :No such channel" );
        return;
    }

    // Eğer kullanıcı kanalda değilse
    if (!channel->isUserOnChannel(client))
    {
        client->sendMessage( ":" + client->getHostName() + " 442 " + client->getNickName() + " " + channelName + " :You're not on that channel" );
        return;
    }

    // Kullanıcıyı kanaldan çıkar
    channel->removeUserFromChannel(client);
    client->removeChannel(channel);

    // Diğer kullanıcılara ayrılma mesajını yayınla
    string message = ":" + client->getPrefix() + " PART " + channelName + "\n";
    channel->broadcastMessage(message, client);

    // Kullanıcıya ayrıldığı kanalı bildir
   	client->sendMessage( "You left the channel " + channelName );

    // Eğer kanalda başka kullanıcı yoksa ve kanal varsa
    if (channel->getChannelClientCount() == 0 && srv->channelExists(channelName))
    {
        // Kanalı sil
   		string channelName = channel->getChannelName();
		string message = "Channel " + channelName + " is empty, deleting.\n";
		write( 1, message.c_str(), message.length() );
		srv->removeChannel( channelName );
    }
}
