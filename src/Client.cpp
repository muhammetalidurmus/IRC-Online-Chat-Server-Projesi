#include "../include/Client.hpp"

// İstemci sınıfının kurucu fonksiyonu.
Client::Client(int clientSocketFD, int clientPort, const string& hostName, const string& serverName)
	: _clientSocketFD(clientSocketFD),
	  _clientPort(clientPort),
	  _hostName(hostName),
	  _serverName(serverName),
	  _clientStatus(CLIENT_CONNECTED),
	  _channel(),
	  userAuth(false),
	  _isPass(false),
	  _isOperator(false)
{
}

// İstemci sınıfının yıkıcı fonksiyonu.
Client::~Client()
{
	// İstemcinin bulunduğu kanalları temizle.
	for (std::vector<Channel*>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	_channel.clear();
}

// İstemci sınıfının atama operatörü.
Client& Client::operator=(const Client& copy)
{
	// İstemci özelliklerini kopyala.
	_clientSocketFD = copy._clientSocketFD;
	_clientPort = copy._clientPort;
	_nickName = copy._nickName;
	_userName = copy._userName;
	_realName = copy._realName;
	_hostName = copy._hostName;
	_serverName = copy._serverName;
	_clientStatus = copy._clientStatus;
	_channel = copy._channel;
	userAuth = copy.userAuth;
	_isPass = copy._isPass;
	_isOperator = copy._isOperator;
	return *this;
}

// İstemciye ait ön ek bilgisini oluşturur.
string Client::getPrefix() const
{
	// Kullanıcı adı veya host adı boşsa "*" olarak atanır.
	string username = _userName.empty() ? "*" : _userName;
	string hostname = _hostName.empty() ? "*" : _hostName;

	// Ön ek oluşturulur ve döndürülür.
	return _nickName + "!" + username + "@" + hostname;
}

// İstemciye bir takma ad atar, geçerli bir takma ad değilse hata mesajı yazdırır.
void Client::setNickName(const string& nickName)
{
	if (isValidName(nickName))
		_nickName = nickName;
	else
	{
		string error = "Invalid nickname: " + nickName;
		write(2, error.c_str(), error.length());
	}
}

// İstemciye bir kullanıcı adı atar, geçerli bir kullanıcı adı değilse hata mesajı yazdırır.
void Client::setUserName(const string& userName)
{
	if (isValidName(userName))
		_userName = userName;
	else
	{
		string error = "Invalid username: " + userName;
		write(2, error.c_str(), error.length());
	}
}

// İstemciye hoş geldin mesajını gönderir.
void Client::welcomeMessage()
{
	// Geçerli bir bağlantı durumu ve takma ad, kullanıcı adı, gerçek ad bilgileri var mı kontrol edilir.
	if (_clientStatus != CLIENT_CONNECTED || _nickName.empty() || _userName.empty() || _realName.empty())
	{
		sendReply("ERROR :Closing Link: " + _hostName + " (Invalid nickname or username)");
		return;
	}

	// İstemci durumu CLIENT_REGISTERED olarak güncellenir.
	_clientStatus = CLIENT_REGISTERED;

	// Hoş geldin mesajı gönderilir.
	sendReply(WELCOME_MESSAGE(_serverName, _nickName));

	// Log için mesaj oluşturulup log fonksiyonu çağrılır.
	std::ostringstream oss;
	oss << _hostName << ":" << _clientPort << " is now known as " << _nickName << ".";
	log(oss.str());
}

// İstemciye bir mesaj gönderir.
void Client::sendMessage(const string& message) const
{
	// Mesajın sonuna "\r\n" eklenir.
	string buffer = message + "\r\n";

	// Sokete mesaj gönderilir, hata durumunda loglanır.
	if (send(_clientSocketFD, buffer.c_str(), buffer.length(), 0) == -1)
		ErrorLogger(FAILED_SOCKET_SEND, __FILE__, __LINE__);
}

// İstemciye bir yanıt gönderir.
void Client::sendReply(const string& reply) const
{
	// Önek ile birleştirilmiş yanıt gönderilir.
	sendMessage(getPrefix() + " " + reply);
}

// İstemciyi çıkar.
void Client::leave()
{
	// İstemci durumu CLIENT_DISCONNECTED olarak güncellenir.
	_clientStatus = CLIENT_DISCONNECTED;
}

// İstemciyi bir kanala ekler.
void Client::join(Channel* channel)
{
	// Kanala istemci eklenir.
	channel->addClient(this);
	// İstemcinin bulunduğu kanallara eklenir.
	_channel.push_back(channel);

	// Kanaldaki tüm kullanıcıların takma adları alınır.
	std::string nickList;
	std::vector<std::string> nicknames = channel->getChannelClientNickNames();
	for (std::vector<std::string>::iterator it = nicknames.begin(); it != nicknames.end(); ++it)
	{
		nickList += *it + " ";
	}

	// RPL_NAMREPLY yanıtı gönderilir.
	sendReply(RPL_NAMREPLY(getPrefix(), channel->getChannelName(), nickList));

	// Kanala katılma mesajı loglanır.
	channel->broadcastMessage(RPL_JOIN(getPrefix(), channel->getChannelName()));
	string message = _nickName + " " + " has joined to the channel " + channel->getChannelName();
	log(message);
}

// İstemcinin bulunduğu kanalı kaldırır.
void Client::removeChannel(Channel* channel)
{
	// Kanalı vektörden çıkar.
	std::vector<Channel*>::iterator it = std::find(_channel.begin(), _channel.end(), channel);

	if (it != _channel.end())
		_channel.erase(it);
}
