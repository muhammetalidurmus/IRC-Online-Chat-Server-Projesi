#include "../../include/Commands.hpp"

// MODE komutunu işleyen fonksiyon
void Mode::mode(Client *client, vector<string> commandParts, Server *srv)
{
    // Komut parametreleri yetersizse hata mesajı gönder
    if (commandParts.size() < 2)
    {
        client->sendMessage(":" + client->getHostName() + " 461 " + client->getNickName() + " MODE :Not enough parameters");
        return;
    }

    // MODE komutu için hedef ve modu al
    string atargetE = commandParts.at(1);
    string mode = (commandParts.size() > 2) ? commandParts.at(2) : "";
    string modeParams = "100";

    // Hedef boşsa hata mesajı gönder
    if (atargetE.empty())
    {
        client->sendMessage(":" + client->getHostName() + " 403 " + client->getNickName() + " " + atargetE + " :No such channel\r\n");
        return;
    }
    else if (atargetE.at(0) == '#')
    {
        // Kanal hedefi ise
        string atarget = atrim(atargetE);
        Channel *channel = srv->getChannel(atarget);

        // Kanal bulunamazsa hata mesajı gönder
        if (!channel)
        {
            client->sendMessage(":" + client->getHostName() + " 403 " + client->getNickName() + " " + atargetE + " :No such channel\r\n");
            return;
        }

        // Mod boşsa veya modun ilk karakteri '+' değilse hata mesajı gönder
        if (mode.empty())
            return;
        if (mode.at(0) == '+')
        {
            // MODE + komutu ise
            if (commandParts.size() > 3)
            {
                modeParams = commandParts.at(3);
                channel->setUpModeChannel(channel, client, mode, modeParams);
            }
            else
                channel->setUpModeChannel(channel, client, mode, modeParams);
        }
        else if (mode.at(0) == '-')
        {
            // MODE - komutu ise
            if (commandParts.size() > 3)
            {
                modeParams = commandParts.at(3);
                channel->setLowModeChannel(channel, client, mode, modeParams);
            }
            else
                channel->setLowModeChannel(channel, client, mode, modeParams);
        }
        else
            client->sendMessage(":" + client->getHostName() + " 501 " + client->getNickName() + " :Unknown MODE flag");
    }
    else
        client->sendMessage(":" + client->getHostName() + " 501 " + client->getNickName() + " :Unknown MODE flag");
}






