#include "../../include/Commands.hpp"

// QUIT komutunu işleyen fonksiyon
void Quit::quit(Client *client, vector<string> commandParts, Server *srv)
{
    // Eğer komut parametreleri boşsa, varsayılan bir ayrılma nedeni kullan
    string reason = commandParts.empty() ? "Leaving from Server" : commandParts.at(0);

    // Eğer ayrılma nedeni ":" ile başlıyorsa, ilk karakteri kaldır
    if (reason.at(0) == ':')
        reason.substr(1);

    // Kullanıcıya ayrılma mesajını gönder
    client->sendMessage(RPL_QUIT(client->getPrefix(), reason));

    // Kullanıcıyı tüm kanallardan kaldır ve bağlantısını kopar
    srv->removeClientFromAllChannels(client);
    srv->clientDisconnect(client->getClientSocketFD());
}
