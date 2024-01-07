#include "../include/Server.hpp"

// Kanalı kaldırma fonksiyonu
void Server::removeChannel(const std::string& channelName)
{
	// Kanalın haritasında belirtilen adı ile kanalı bul.
	std::map<std::string, Channel*>::iterator it = _channels.find(channelName);

	// Kanal bulundu mu kontrol et.
	if (it != _channels.end())
	{
		Channel* channel = it->second;

		// Kanal var mı ve NULL değil mi kontrol et.
		if (channel != NULL)
		{
			// Kanalı sil.
			delete channel;
			_channels.erase(it);
		}
		else
		{
			string message = "Channel " + channelName + " is already null.";
			write(STDOUT_FILENO, message.c_str(), message.size());
		}
	}
	else
	{
		string message = "Channel " + channelName + " does not exist.";
		write(STDOUT_FILENO, message.c_str(), message.size());
	}
}

// İstemciyi tüm kanallardan çıkar.
void Server::removeClientFromAllChannels(Client* client)
{
	// Geçerli bir istemci veya açık soket olup olmadığını kontrol et.
	if (client == NULL || !client->isSocketOpen())
		return;

	// İstemcinin bulunduğu tüm kanalların bir kopyasını oluşturun ve istemciyi kanallardan kaldırın.
	std::vector<Channel*> channels = client->getChannel();
	while (!channels.empty())
	{
		Channel* channel = channels.back();
		channels.pop_back();

		// Kanal var mı kontrol et.
		if (channel != NULL)
		{
			// Kanal ve istemciyi birbirinden çıkar.
			string channelName = channel->getChannelName();
			string clientNick = client->getNickName();
			channel->removeUserFromChannel(client);
			client->removeChannel(channel);

			// Log mesajı oluştur.
			string leaveMessage = clientNick + " has left the channel " + channelName;
			log(leaveMessage);

			// Eğer kanalda hiç kullanıcı kalmamışsa ve kanal hala varsa, kanalı kaldır.
			if (channel->getChannelClientCount() == 0 && channelExists(channel->getChannelName()))
			{
				string message = "Channel " + channelName + " is empty, deleting.\n";
				write(STDOUT_FILENO, message.c_str(), message.size());
				removeChannel(channelName);
			}
		}
	}
}

// Kanal ekleyen fonksiyon
void Server::addChannel(Channel* channel)
{
	_channels.insert(std::make_pair(channel->getChannelName(), channel));
}

// Belirli bir kanalın var olup olmadığını kontrol eden fonksiyon
bool Server::channelExists(const string& channelName)
{
	// Kanallar haritasında belirtilen ad ile kanalı bul.
	for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->second->getChannelName() == channelName)
			return true;
	}
	return false;
}

// Belirli bir istemciyi nickName'e göre bulan fonksiyon
Client* Server::getClient(string& nickName)
{
	// İstemciler haritasında belirtilen nickName ile istemciyi bul.
	for (map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickName() == nickName)
			return it->second;
	}
	return NULL;
}

// Belirli bir kanalı channelName'e göre bulan fonksiyon
Channel* Server::getChannel(string& channelName)
{
	// Kanallar haritasında belirtilen ad ile kanalı bul.
	for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->second->getChannelName() == channelName)
			return it->second;
	}
	return NULL;
}
