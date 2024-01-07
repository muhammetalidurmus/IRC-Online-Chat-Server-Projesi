#include "../../include/Commands.hpp"

// PRIVMSG komutunu işleyen fonksiyon
void PrivMsg::privMsg(Client *client, vector<string> commandParts, Server *srv)
{
    // Komut parametreleri yetersizse hata mesajı gönder
    if (commandParts.size() < 2)
    {
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "PRIVMSG"));
        return;
    }

    // Komut stringini birleştir
    string commandString = mergeString(commandParts, " ");

    // Hedefin başlangıç konumunu ve mesajın başlangıç konumunu bul
    size_t targetStart = commandString.find("PRIVMSG") + 8;
    size_t messageStart = commandString.find(" :", targetStart);

    // Eğer mesaj yoksa hata mesajı gönder
    if (messageStart == string::npos)
    {
        client->sendReply(ERR_NOTEXTTOSEND(client->getNickName()));
        return;
    }

    // Hedef ve mesajı ayır
    string target = commandString.substr(targetStart, messageStart - targetStart);
    target = strim(target);
    string message = commandString.substr(messageStart + 2);

    // Eğer hedef bir kanalsa, kanal mesajını gönder
    if (target.at(0) == '#')
    {
        sendChannelMessage(client, target, message, srv);
    }
    else
    {
        // Hedef bir kullanıcıysa özel mesajı gönder
        sendPrivateMessage(client, target, message, srv);
    }
}

// Kanal mesajını gönderen yardımcı fonksiyon
void sendChannelMessage(Client *client, string channelName, string message, Server *srv)
{
    // Kanalı al
    Channel *channel = srv->getChannel(channelName);

    // Eğer kanal bulunamazsa hata mesajı gönder
    if (channel == NULL)
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
        return;
    }

    // Eğer kanal moderasyona tabi ise ve kullanıcı operatör değilse hata mesajı gönder
    if (channel->getModerated() && !client->isOperator())
    {
        client->sendReply(ERR_NOCANNOTSENDTOCHAN(client->getNickName(), channelName));
        return;
    }

    // Eğer kanal dışından mesaj alınmaması gerekiyorsa ve kullanıcı kanalda değilse hata mesajı gönder
    if (channel->getNoExternalMessages() && !channel->isUserOnChannel(client))
    {
        client->sendReply(ERR_NOCANNOTSENDTOCHAN(client->getNickName(), channelName));
        return;
    }

    // Kanal mesajını yayınla
    channel->broadcastMessage(":" + client->getPrefix() + " PRIVMSG " + channelName + " :" + message, client);
}

// Özel mesajı gönderen yardımcı fonksiyon
void sendPrivateMessage(Client *client, string target, string message, Server *srv)
{
    // Hedef kullanıcıyı al
    Client *targetClient = srv->getClient(target);

    // Eğer hedef kullanıcı bulunamazsa hata mesajı gönder
    if (targetClient == NULL)
    {
        client->sendReply(ERR_NOSUCHNICK(client->getNickName(), target));
        return;
    }

    // Özel mesajı hedef kullanıcıya gönder
    targetClient->sendMessage(":" + client->getPrefix() + " PRIVMSG " + target + " :" + message);
}
