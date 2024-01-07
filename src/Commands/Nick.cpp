#include "../../include/Commands.hpp"

// NICK komutunu işleyen fonksiyon
void Nick::nick(Client *client, vector<string> commandParts, Server *srv)
{
    // Komut parametreleri yetersizse hata mesajı gönder
    if (commandParts.size() < 2)
    {
        client->sendReply(ERR_NONICKNAMEGIVEN(client->getNickName()));
        return;
    }

    // Yeni takma adı al
    string nickName = commandParts.at(1);

    // Takma adın geçerli olup olmadığını kontrol et
    if (!client->getValidName(nickName))
    {
        client->sendReply(ERR_ERRONEUSNICKNAME(client->getNickName(), nickName));
        return;
    }

    // Yeni takma adın şu anki takma adı ile aynı olup olmadığını kontrol et
    if (client->getNickName() == nickName)
    {
        client->sendReply(ERR_NICKNAMEINUSE(client->getNickName(), nickName));
        return;
    }

    // Sunucudaki tüm kullanıcılarda aynı takma adın kullanılıp kullanılmadığını kontrol et
    const std::map<int, Client *> &clients = srv->getAllClients();
    for (std::map<int, Client *>::const_iterator it = clients.begin(); it != clients.end(); ++it)
    {
        const Client *regUser = it->second;
        if (!regUser)
        {
            continue;
        }
        if (regUser != client && regUser->getNickName() == nickName)
        {
            client->sendReply(ERR_NICKNAMEINUSE(client->getNickName(), nickName));
            return;
        }
    }

    // Takma adı değiştir ve mesaj gönder
    client->sendMessage(":" + client->getNickName() + " NICK " + nickName);
    client->setNickName(nickName);
}
