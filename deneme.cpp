#include "../../include/Commands.hpp" // Yol: include/Commands.hpp

// Kick komutunun uygulaması
void Kick::kick(Client* client, vector<string> commandParts, Server* srv)
{
    // Yeterli parametrelerin sağlandığını kontrol et
    if (commandParts.size() < 3)
    {
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "KICK"));
        return;
    }

    // KomutParts vektöründen parametreleri çıkar
    string channelNamee = commandParts.at(1);
    string nickName = commandParts.at(2);
    string reason = (commandParts.size() > 3) ? commandParts.at(3) : "No reason specified";

    // Kanal adının '#' ile başlayıp başlamadığını kontrol et
    if (channelNamee.at(0) != '#')
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelNamee));
        return;
    }

    // Sunucudan kanal nesnesini al
    Channel* channel = srv->getChannel(channelNamee);
    if (!channel)
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelNamee));
        return;
    }

    // İstemcinin belirtilen kanalda olup olmadığını kontrol et
    if (!channel->isUserOnChannel(client))
    {
        client->sendReply(ERR_NOTONCHANNEL(client->getNickName(), channelNamee));
        return;
    }

    // Hedef istemciyi sunucudan al
    Client* target = srv->getClient(nickName);
    if (!target)
    {
        client->sendReply(ERR_NOSUCHNICK(client->getNickName(), nickName));
        return;
    }

    // Hedef istemcinin belirtilen kanalda olup olmadığını kontrol et
    if (!channel->isUserOnChannel(target))
    {
        client->sendReply(ERR_USERNOTINCHANNEL(client->getNickName(), nickName, channelNamee));
        return;
    }

    // Kanal adını temizle
    string channelName = atrim(channelNamee);

    // Kanal sahibini kontrol et ve sahibe kick atılmaya çalışılıyorsa hata mesajı gönder
    if (channel->getChannelOwner()->getNickName() == target->getNickName())
    {
        client->sendReply(ERR_NOKICKCHANNELOWNER(client->getNickName(), channelName));
        return;
    }

    // İstemci operatörse, hedef istemciyi kanaldan çıkar ve bildirim mesajlarını gönder
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
        // İstemci operatör değilse, hata mesajı gönder
        client->sendReply(ERR_CHANOPRIVSNEEDED(client->getNickName(), channelName));
        return;
    }

    // Eğer kanaldaki toplam istemci sayısı 0 ise, kanalı kaldır
    if (channel->getChannelClientCount() == 0)
    {
        string channelName = channel->getChannelName();
        srv->removeChannel(channelName);
    }
}

