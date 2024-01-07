#include "../include/Channel.hpp"

// Kanal sınıfının yapıcı fonksiyonu
Channel::Channel(const string& channelName, const string& channelKey, Client* channelOwner)
	: _channelName(channelName),
	  _channelOwner(channelOwner),
	  _channelKey(channelKey),
	  _userLimit(1000),
	  _noExternalMessages(false),
	  _moderated(false)
{
}

// Kanal sınıfının yıkıcı fonksiyonu
Channel::~Channel()
{
}

// Kanala mesajı yayınlayan fonksiyon
void Channel::broadcastMessage(const string& message) const
{
	string tmp = message;
	for (vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		(*it)->sendMessage(message);
	}
}

// Belirli bir istemici hariç tutarak kanala mesajı yayınlayan fonksiyon
void Channel::broadcastMessage(const string& message, Client* exceptClient) const
{
	for (vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it != exceptClient)
			(*it)->sendMessage(message);
	}
}

// Kanala istemci ekleyen fonksiyon
void Channel::addClient(Client* client)
{
	_clients.push_back(client);
}

// Kanaldan istemciyi kaldıran fonksiyon
void Channel::removeUserFromChannel(Client* client)
{
	for (vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			_clients.erase(it);
			break;
		}
	}

	// Kanal sahibi bir operatörse ve operatör olan tek kişi çıkarsa, yeni bir operatör belirle.
	if (client->isOperator() == true)
	{
		client->setOperator(false);
		for (vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if ((*it)->isOperator() == true)
				return;
		}

		// Eğer kanalda hala istemci varsa, ilk istemciyi yeni operatör yap.
		if (_clients.size() > 0)
		{
			Client* newOperator = _clients.front();
			if (newOperator->isOperator() == false)
			{
				newOperator->setOperator(true);
				setChannelOwner(newOperator);
				broadcastMessage("MODE " + _channelName + " +o " + newOperator->getNickName());
			}
		}
	}
}

// Bir istemcinin kanalda olup olmadığını kontrol eden fonksiyon
bool Channel::isUserOnChannel(Client* client) const
{
	for (vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
			return true;
	}
	return false;
}

// Kanalın modlarını ayarlayan fonksiyon
void Channel::setUpModeChannel(Channel* channel, Client* client, string& mode, string& modeParams)
{
	if (!channel || !client)
	{
		write(2, "Channel or Client is null, cannot set mode.", 43);
		return;
	}
	if (mode == "+k" && (client->isOperator() == true))
	{
		channel->setChannelKey(modeParams);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +k " + client->getNickName());
	}
	else if (mode == "+l" && (client->isOperator() == true))
	{
		channel->setUserLimit(atoi(modeParams.c_str()));
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +l " + client->getNickName());
	}
	else if (mode == "+o" && (client->isOperator() == true))
	{
		Client* targetClient = Server::getInstance()->getClient(modeParams);
		if (!targetClient)
		{
			client->sendMessage(":" + client->getHostName() + " 401 " + client->getNickName() + " " + modeParams + " :No such nick/channel\r\n");
			return;
		}
		targetClient->setOperator(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +o " + targetClient->getNickName());
	}
	else if (mode == "+m" && client->isOperator() == true)
	{
		channel->setModerated(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +m " + client->getNickName());
	}
	else if (mode == "+n" && (client->isOperator() == true))
	{
		channel->setNoExternalMessages(true);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " +n " + client->getNickName());
	}
	else
	{
		client->sendMessage(":" + client->getHostName() + " 501 " + client->getNickName() + " :Unknown MODE flag");
	}
}

// Kanalın düşük modlarını ayarlayan fonksiyon
void Channel::setLowModeChannel(Channel* channel, Client* client, string& mode, string& modeParams)
{
	if (!channel || !client)
	{
		write(2, "Channel or Client is null, cannot set mode.", 43);
		return;
	}
	if (mode == "-k" && (client->isOperator() == true))
	{
		channel->setChannelKey("");
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -k " + client->getNickName());
	}
	else if (mode == "-l" && (client->isOperator() == true))
	{
		channel->setUserLimit(1000);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -l " + client->getNickName());
	}
	else if (mode == "-n" && (client->isOperator() == true))
	{
		channel->setNoExternalMessages(false);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -n " + client->getNickName());
	}
	else if (mode == "-o" && (client->isOperator() == true))
	{
		Client* targetClient = Server::getInstance()->getClient(modeParams);
		if (!targetClient)
		{
			client->sendMessage(":" + client->getHostName() + " 401 " + client->getNickName() + " " + modeParams + " :No such nick/channel\r\n");
			return;
		}
		if (targetClient->getNickName() != channel->getChannelOwner()->getNickName())
		{
			targetClient->setOperator(false);
			channel->broadcastMessage("MODE " + channel->getChannelName() + " -o " + targetClient->getNickName());
		}
	}
	else if (mode == "-m" && client->isOperator() == true)
	{
		channel->setModerated(false);
		channel->broadcastMessage("MODE " + channel->getChannelName() + " -m " + client->getNickName());
	}
	else
		client->sendMessage(":" + client->getHostName() + " 501 " + client->getNickName() + " :Unknown MODE flag");
}

// Kanaldaki istemcilerin nickName'lerini içeren vektörü döndüren fonksiyon
std::vector<std::string> Channel::getChannelClientNickNames() const
{
	vector<string> nickNames;
	for (vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		nickNames.push_back((*it)->getNickName());
	}
	return nickNames;
}

// Kanaldaki mevcut istemcilerin nickName'lerini içeren bir stringi döndüren fonksiyon
string Channel::getExistingUsersNickList() const
{
	string nickList;
	for (std::vector<Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it != _clients.begin())
			nickList += " ";

		nickList += (*it)->getNickName();
	}
	return nickList;
}
