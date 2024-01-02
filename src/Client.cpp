#include "../include/Client.hpp"

Client::Client( int clientSocketFD, int clientPort, const string& hostName, const string& serverName )
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

Client::~Client()
{
	for (std::vector<Channel*>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		delete *it;
		*it = NULL;
	}
	_channel.clear();
}

Client& Client::operator=( const Client& copy )
{
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

string Client::getPrefix() const
{
	string username = _userName.empty() ? "*" : _userName;
	string hostname = _hostName.empty() ? "*" : _hostName;

	return _nickName + "!" + username + "@" + hostname;
}

void Client::setNickName( const string& nickName ) {
	if (isValidName(nickName))
		_nickName = nickName;
	else {
		string a = "Invalid nickname: " + nickName;
		write(2, a.c_str(), a.length());
	}
}

void Client::setUserName( const string& userName ) {
	if (isValidName(userName))
		_userName = userName;
	else {
		string a1 = "Invalid username: " + userName;
		write(2, a1.c_str(), a1.length());
	}
}

void Client::welcomeMessage()
{
	if ( _clientStatus != CLIENT_CONNECTED || _nickName.empty() || _userName.empty() || _realName.empty() )
	{
		sendReply("ERROR :Closing Link: " + _hostName + " (Invalid nickname or username)");
		return;
	}

	_clientStatus = CLIENT_REGISTERED;
	sendReply(WELCOME_MESSAGE(_serverName, _nickName));

	std::ostringstream oss;
	oss << _hostName << ":" << _clientPort << " is now known as " << _nickName << ".";
	log(oss.str());
}

void Client::sendMessage( const string& message ) const
{
	string buffer = message + "\r\n";

	if (send(_clientSocketFD, buffer.c_str(), buffer.length(), 0) == -1)
		ErrorLogger(FAILED_SOCKET_SEND, __FILE__, __LINE__);
}

void Client::sendReply( const string& reply ) const
{
	sendMessage(getPrefix() + " " + reply);
}

void Client::leave()
{
	_clientStatus = CLIENT_DISCONNECTED;
}

void Client::join( Channel* channel )
{
	channel->addClient(this);
	_channel.push_back(channel);

	std::string nickList;
	std::vector<std::string> nicknames = channel->getChannelClientNickNames();
	for (std::vector<std::string>::iterator it = nicknames.begin(); it != nicknames.end(); ++it) {
		nickList += *it + " ";
	}

	sendReply(RPL_NAMREPLY(getPrefix(), channel->getChannelName(), nickList));

	channel->broadcastMessage(RPL_JOIN(getPrefix(), channel->getChannelName()));
	string message = _nickName + " " + " has joined to the channel " + channel->getChannelName();
	log(message);
}

void Client::removeChannel(Channel* channel) {
	std::vector<Channel*>::iterator it = std::find(_channel.begin(), _channel.end(), channel);

	if (it != _channel.end())
		_channel.erase(it);
}
