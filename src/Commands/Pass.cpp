#include "../../include/Commands.hpp"

// PASS komutunu işleyen fonksiyon
void Pass::pass(Client *client, const vector<string> commandParts, Server *srv)
{
    // Eğer kullanıcı zaten kayıtlı veya kimlik doğrulama yapıldıysa veya PASS komutu zaten alındıysa hata mesajı gönder
    if (client->isRegistered() || client->getUserAuth() || client->getIsPass())
    {
        client->sendReply(ERR_ALREADYREGISTERED(client->getNickName()));
        return;
    }

    // Komut parametreleri yetersizse hata mesajı gönder
    if (commandParts.size() < 2)
    {
        client->sendReply(ERR_NEEDMOREPARAMS(client->getNickName(), "PASS"));
        return;
    }

    // Komutun ikinci parçasını al
    string passw = commandParts.at(1);

    // Sunucu şifresini doğrula
    if (!srv->verifySrvPass(passw))
    {
        // Şifre doğrulanamazsa erişim reddedildiğine dair mesaj gönder
        client->sendMessage("Access denied!");
        return;
    }

    // Kullanıcıya PASS komutunun alındığına dair işareti koy
    client->setPass(true);
    // Kullanıcıya şifrenin kabul edildiğine dair mesaj gönder
    client->sendMessage("Password accepted!");
}
