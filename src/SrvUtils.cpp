#include "../include/Server.hpp"

void Server::processPartialCommands(int clientSocketFD)
{
	string& clientBuffer = clientBuffers[clientSocketFD].getBuffer();
	size_t endOfCommand;
	string command;
	if (clientBuffer[0] == '/')
	{
		while ((endOfCommand = clientBuffer.find('\n')) != string::npos)
		{
			command = clientBuffer.substr(0, endOfCommand);
			clientBuffer.erase(0, endOfCommand + 1);
			CommandParser::commandParser(command.c_str(), _clients[clientSocketFD], this);
		}
	}
	else
	{
		while ((endOfCommand = clientBuffer.find("\r\n")) != string::npos) {
			command = clientBuffer.substr(0, endOfCommand);
			clientBuffer.erase(0, endOfCommand + 1);
			CommandParser::commandParser(command.c_str(), _clients[clientSocketFD], this);
		}
	}
}
// Handling a client request
// Receives the file descriptor of the client socket and processes the request.
void Server::handleClient(int clientSocketFD)
{
	if (clientSocketFD == _bot->getSocket())
	{
		_bot->listen();
	}
	else
	{
 		const size_t BUFFER_SIZE = 512;
		char tempBuffer[BUFFER_SIZE];
		memset(tempBuffer, 0, BUFFER_SIZE);

		ssize_t received = recv(clientSocketFD, tempBuffer, BUFFER_SIZE - 1, 0);
		if (received > 0) {
			clientBuffers[clientSocketFD].appendtoBuffer(string(tempBuffer, received));
			processPartialCommands(clientSocketFD);
		} else if (received == 0 || errno == ECONNRESET) {
			clientDisconnect(clientSocketFD);
			clientBuffers.erase(clientSocketFD);
		} else {
			if (errno != EAGAIN && errno != EWOULDBLOCK) {
				ErrorLogger("recv error", __FILE__, __LINE__);
				close(clientSocketFD);
				clientBuffers.erase(clientSocketFD);
			}
		}
	}
}
// Disconnecting a client
// Client disconnect all channels and close the connection
void Server::clientDisconnect(int clientSocketFD)
{
    try
    {
        std::map<int, Client*>::iterator it = _clients.find(clientSocketFD);
        if (it == _clients.end()) {
            write(STDOUT_FILENO, "Client not found for removal.\n", 30);
            return;
        }
        removeClientFromAllChannels(it->second);
        it->second->leave();
        ostringstream messageStreamDisconnect;
        messageStreamDisconnect << "Client " << it->second->getNickName() << " has disconnected.";
        log(messageStreamDisconnect.str());

        struct kevent evSet;
        EV_SET(&evSet, clientSocketFD, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        kevent(kq, &evSet, 1, NULL, 0, NULL);

        close(clientSocketFD);
        delete it->second;
        _clients.erase(it);
    }
    catch (const std::exception &e)
    {
        write(STDOUT_FILENO, e.what(), strlen(e.what()));
    }
}

void Server::setSrvPass(const string& pass) {
	_serverPass = pass;
}

bool Server::verifySrvPass(const string& pass) {
	if (_serverPass == pass) {
		return true;
	}
	return false;
}
// Handle the signal and close the server.
void Server::signalHandler(int signum)
{
	Server::getInstance()->shutdownSrv();
	exit(signum);
}
// All channels are removed from the client and the connection is closed.
void Server::shutdownSrv()
{
	string outmessage = "Sunucu kapatılıyor...\n";
	write(STDOUT_FILENO, outmessage.c_str(), outmessage.size());

	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		Client* client = it->second;
		if (client != NULL) {
			client->sendMessage("Sunucu kapatılıyor. Bağlantınız sonlandırılıyor.");
			removeClientFromAllChannels(client);
			close(it->first);
			delete client;
		}
	}
	_clients.clear();

	if (_serverSocketFD != -1) {
		close(_serverSocketFD);
		_serverSocketFD = -1;
	}

	if (_bot != NULL) {
		delete _bot;
		_bot = NULL;
	}

	if (kq != -1)
		close(kq);

	system("leaks ircserv");
	string outmessage2 = "Sunucu kapatıldı.\n";
	write(STDOUT_FILENO, outmessage2.c_str(), outmessage2.size());
}
