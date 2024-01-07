#include "../../include/Commands.hpp"

void Kick::kick(Client* client, vector<string> commandParts, Server* srv)
{
    // Kontrol: Komut parçalarının sayısı yetersizse hata gönder ve işlemi sonlandır
    if (commandParts.size() < 3)
    {
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "KICK"));
        return;
    }

    // Komut parçalarını tanımla
    string channelNamee = commandParts.at(1);
    string nickName = commandParts.at(2);
    string reason = (commandParts.size() > 3) ? commandParts.at(3) : "No reason specified";

    // Kontrol: Kanal adı '#' ile başlamıyorsa hata gönder ve işlemi sonlandır
    if (channelNamee.at(0) != '#')
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelNamee));
        return;
    }

    // Kanalı al veya varsa hata gönder ve işlemi sonlandır
    Channel* channel = srv->getChannel(channelNamee);
    if (!channel)
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelNamee));
        return;
    }

    // Kullanıcının kanalda olup olmadığını kontrol et
    if (!channel->isUserOnChannel(client))
    {
        client->sendReply(ERR_NOTONCHANNEL(client->getNickName(), channelNamee));
        return;
    }

    // Hedef kullanıcıyı al veya varsa hata gönder ve işlemi sonlandır
    Client* target = srv->getClient(nickName);
    if (!target)
    {
        client->sendReply(ERR_NOSUCHNICK(client->getNickName(), nickName));
        return;
    }

    // Hedef kullanıcının kanalda olup olmadığını kontrol et
    if (!channel->isUserOnChannel(target))
    {
        client->sendReply(ERR_USERNOTINCHANNEL(client->getNickName(), nickName, channelNamee));
        return;
    }

    // Kanal sahibi hedef kullanıcıysa hata gönder ve işlemi sonlandır
    if (channel->getChannelOwner()->getNickName() == target->getNickName())
    {
        client->sendReply(ERR_NOKICKCHANNELOWNER(client->getNickName(), channelNamee));
        return;
    }

    // Operatör yetkisine sahipse işlemi gerçekleştir, aksi takdirde hata gönder ve işlemi sonlandır
    if (client->isOperator())
    {
        // Kullanıcıyı kanaldan at
        channel->removeUserFromChannel(target);
        target->removeChannel(channel);

        // Kick mesajını oluştur ve kanala yayınla
        string message = ":" + client->getPrefix() + " KICK " + channelNamee + " " + nickName + " :" + reason + "\n";
        channel->broadcastMessage(message, client);

        // Hedef kullanıcıya kick mesajını gönder
        target->sendMessage(":" + client->getPrefix() + " KICK " + channelNamee + " " + nickName + " :" + reason);
        target->sendMessage("You have been kicked from channel " + channelNamee + " by " + client->getNickName());

        // Log mesajını oluştur ve kaydet
        string leavemessage = client->getNickName() + " kicked " + target->getNickName() + " from channel " + channel->getChannelName();
        log(leavemessage);
    }
    else
    {
        // Operatör yetkisi yoksa hata gönder ve işlemi sonlandır
        client->sendReply(ERR_CHANOPRIVSNEEDED(client->getNickName(), channelNamee));
        return;
    }

    // Kanalın kullanıcı sayısı 0 ise kanalı kaldır
    if (channel->getChannelClientCount() == 0)
    {
        string channelName = channel->getChannelName();
        srv->removeChannel(channelName);
    }
}
