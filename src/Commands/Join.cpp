#include "../../include/Commands.hpp"

// Join sınıfının join fonksiyonu
void Join::join(Client* client, vector<string> commandParts, Server* srv)
{
    // Eğer komut parametreleri yetersizse
    if (commandParts.size() < 2)
    {
        // Hata mesajı gönder ve fonksiyondan çık
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "JOIN"));
        return;
    }

     // Oda adını atama yapar.
    string channel = commandParts.at(1);

    // Eğer oda adı varsa oda açma fonksiyonuna gönderir.
    if (!channel.empty())
        joinChannel(client, channel, commandParts, srv);
        
}

// Join sınıfının joinChannel fonksiyonu
void Join::joinChannel(Client* client, string channelName, vector<string> commandParts, Server* srv)
{
    string channelPass;

    // Eğer kanal adı boşsa
    if (channelName.empty())
    {
        // Hata mesajı gönder ve fonksiyondan çık
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
        return;
    }

    // Eğer sadece kanal adı ve şifre varsa
    if (commandParts.size() == 2)
        channelPass = "";
    else
        channelPass = commandParts.at(2);

    // Kanal adının uzunluğu ve başındaki karakter kontrolü
    if (channelName.size() < 2 || channelName.at(0) != '#')
    {
        // Hata mesajı gönder ve fonksiyondan çık
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
        return;
    }

    Channel* channel;

    // Eğer kanal zaten varsa
    if (srv->channelExists(channelName))
    {
        // Kanal nesnesini al
        channel = srv->getChannel(channelName);

        // Eğer istemci zaten bu kanalda bulunuyorsa
        if (channel->isUserOnChannel(client))
        {
            // Hata mesajı gönder ve fonksiyondan çık
            client->sendReply(ERR_USERONCHANNEL(client->getNickName(), channelName));
            return;
        }
        // Eğer kanal şifresi doğru değilse
        else if (channel->getChannelKey() != channelPass)
        {
            // Hata mesajı gönder ve fonksiyondan çık
            client->sendReply(ERR_BADCHANNELKEY(client->getNickName(), channelName));
            return;
        }
        // Eğer kanalın kullanıcı sınırına ulaşıldıysa
        else if (channel->getUserLimit() <= channel->getChannelClientCount())
        {
            // Hata mesajı gönder ve fonksiyondan çık
            client->sendReply(ERR_CHANNELISFULL(client->getNickName(), channelName));
            return;
        }
    }
    // Eğer kanal daha önce oluşturulmamışsa
    else
    {
        // Yeni bir kanal nesnesi oluştur
        channel = new Channel(channelName, channelPass, client);
        channel->setChannelOwner(client);
        client->setOperator(true);
        srv->addChannel(channel);
        // Kanalı +n moduyla açmak için yazdım böylece kanal dışardan msj almayacak şekilde açılıyor.
        channel->setNoExternalMessages(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +n " + client->getNickName());
    }

    // İstemciyi kanala kat
    client->join(channel);
}
