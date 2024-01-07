#include "../../include/Commands.hpp"

// Belirtilen bir kanala NOTICE mesajı gönderen yardımcı fonksiyon
static void sendNoticeChannelMessage(Client *client, string channelName, string message, Server *srv)
{
    // Belirtilen kanalı sunucudan al
    Channel *channel = srv->getChannel(channelName);

    // Kanal bulunamazsa hata mesajı gönder
    if (channel == NULL)
    {
        client->sendReply(ERR_NOSUCHCHANNEL(client->getNickName(), channelName));
        return;
    }

    // Kanalın tüm kullanıcılarına NOTICE mesajını gönder
    channel->broadcastMessage(":" + client->getPrefix() + " NOTICE " + channelName + " :" + message);
}

// Belirtilen bir kullanıcıya NOTICE mesajı gönderen yardımcı fonksiyon
static void sendNoticeMessage(Client *client, string target, string message, Server *srv)
{
    // Belirtilen kullanıcıyı sunucudan al
    Client *targetClient = srv->getClient(target);

    // Kullanıcı bulunamazsa hata mesajı gönder
    if (targetClient == NULL)
    {
        client->sendReply(ERR_NOSUCHNICK(client->getNickName(), target));
        return;
    }

    // Kullanıcıya NOTICE mesajını gönder
    targetClient->sendMessage(":" + client->getPrefix() + " NOTICE " + target + " :" + message);
}

// NOTICE komutunu işleyen fonksiyon
void Notice::notice(Client *client, vector<string> commandParts, Server *srv)
{
    // Komut parametreleri yetersizse hata mesajı gönder
    if (commandParts.size() < 3)
    {
        client->sendMessage(":" + client->getHostName() + " 461 " + client->getNickName() + " NOTICE :Not enough parameters");
        return;
    }

    // Komutun tamamını içeren bir string oluştur
    string commandString = mergeString(commandParts, " ");
    // Hedefin başlangıç pozisyonunu bul
    size_t targetStart = commandString.find("NOTICE") + 7;
    // Mesajın başlangıç pozisyonunu bul
    size_t messageStart = commandString.find(" :", targetStart);

    // Mesaj bulunamazsa hata mesajı gönder
    if (messageStart == string::npos)
    {
        client->sendMessage(":" + client->getHostName() + " 412 " + client->getNickName() + "  NOTICE :No text to send");
        return;
    }

    // Hedefi ve mesajı al
    string target = commandString.substr(targetStart, messageStart - targetStart);
    string message = commandString.substr(messageStart + 2);

    // Hedef boşsa hata mesajı gönder
    if (target.empty())
    {
        client->sendMessage(":" + client->getHostName() + " 401 " + client->getNickName() + " NOTICE :No such nick/channel");
        return;
    }

    // Mesaj boşsa hata mesajı gönder
    if (message.empty())
    {
        client->sendMessage(":" + client->getHostName() + " 412 " + client->getNickName() + " NOTICE :No text to send");
        return;
    }

    // Hedefi temizle
    target = strim(target);

    // Hedef bir kanal ise kanal mesajını gönder, değilse özel mesajı gönder
    if (target.at(0) == '#')
    {
        sendNoticeChannelMessage(client, target, message, srv);
    }
    else
    {
        sendNoticeMessage(client, target, message, srv);
    }
}
