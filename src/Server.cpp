#include "../include/Server.hpp"

Server* Server::ins = NULL;

// Create a file descriptor for epoll(linux) or kqueue(BSD) to listen for requests.
Server::Server(int serverSocketFamily, int serverSocketProtocol, int serverSocketPort, string serverName)
	: _serverSocketFD(-1),
	_serverSocketFamily(serverSocketFamily),
	_serverSocketProtocol(serverSocketProtocol),
	_serverSocketPort(serverSocketPort),
	_serverName(serverName),
	_serverPass(""),
	_bot(NULL)
{
	signal(SIGINT, signalHandler);
	Server::setInstance(this);
	memset(&serverAddress, 0, sizeof(serverAddress));
	memset(&serverAddress6, 0, sizeof(serverAddress6));

	kq = kqueue();
	if (kq == -1)
		ErrorLogger( FAILED_SOCKET_POLL, __FILE__, __LINE__, true );
}

Server::~Server()
{
	delete Server::ins;
	Server::ins = NULL;
	for (map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	_clients.clear();
	for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete it->second;
	}
	_channels.clear();

	if (_serverSocketFD != -1)
		close(_serverSocketFD);

	if (kq != -1)
		close(kq);

	if (_bot != NULL)
		delete _bot;
}
// Creation of the server socket. The return value is the file identifier of the socket.
// NONBLOCK ->Setting the socket to non-blocking. That is, to read and write to the socket without blocking.
// reuse ->Setting the socket to be reusable.
// reuse -> This is necessary so that the server can be restarted immediately after shutdown.
void Server::socketStart()
{
	_serverSocketFD = socket( _serverSocketFamily, _serverSocketProtocol, 0 );

	if ( _serverSocketFD == -1 )
		ErrorLogger( FAILED_SOCKET, __FILE__, __LINE__ );

	if ( fcntl(_serverSocketFD, F_SETFL, O_NONBLOCK) == -1 )
	{
		close(_serverSocketFD);
		ErrorLogger( FAILED_SOCKET_NONBLOCKING, __FILE__, __LINE__ );
	}
	int reuse = 1;
	if ( setsockopt(_serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1 )
	{
		close(_serverSocketFD);
		ErrorLogger( FAILED_SOCKET_OPTIONS, __FILE__, __LINE__ );
	}
}
// Providing the necessary connection arguments for the socket
void Server::socketInit()
{
	switch ( _serverSocketFamily )
	{
		case AF_INET: // IPv4
			serverAddress.sin_addr.s_addr = INADDR_ANY;
			serverAddress.sin_family = _serverSocketFamily;
			serverAddress.sin_port = htons( _serverSocketPort );
			break;
		case AF_INET6: // IPv6
			serverAddress6.sin6_addr = in6addr_any;
			serverAddress6.sin6_family = _serverSocketFamily;
			serverAddress6.sin6_port = htons( _serverSocketPort );
			break;
		default:
			close(_serverSocketFD);
			ErrorLogger( FAILED_SOCKET_DOMAIN, __FILE__, __LINE__ );
	}
}
// AF_INET - AF_INET6 Binding the address of the server socket for IPv4 or IPv6
// In short, it specifies from which IP address and port the server will listen for requests.
void Server::socketBind()
{
	switch (_serverSocketFamily)
	{
		case AF_INET:
		{
			if ( ::bind(_serverSocketFD, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1 )
			{
				close(_serverSocketFD);
				ErrorLogger(FAILED_SOCKET_BIND, __FILE__, __LINE__);
			}
			break;
		}
		case AF_INET6:
		{
			if ( ::bind(_serverSocketFD, reinterpret_cast<struct sockaddr *>(&serverAddress6), sizeof(serverAddress6)) == -1 )
			{
				close(_serverSocketFD);
				ErrorLogger(FAILED_SOCKET_BIND, __FILE__, __LINE__);
			}
			break;
		}
	}
}
// listen -> Listening on the server socket
// In short, the server starts listening for requests.
// epool-kqueueListening on the server socket with epoll or kqueue
// In short, for multi-client support.
// EPOLLIN-> Readable data is available
// EVFILT_READ-> Readable data is available
void Server::socketListen()
{
	if ( listen(_serverSocketFD, BACKLOG_SIZE) == -1 ){
		close(_serverSocketFD);
		ErrorLogger(FAILED_SOCKET_LISTEN, __FILE__, __LINE__);
	}

	struct kevent evSet;
	EV_SET(&evSet, _serverSocketFD, EVFILT_READ, EV_ADD, 0, 0, NULL);

	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
		perror("kevent: server socket");
}
// Accepting a new client connection
// Receives a new client connection and returns the file descriptor of the new socket.
// EPOLLET -> Edge Triggered: The event is triggered only when the state of the socket changes.
int Server::socketAccept()
{
	struct sockaddr_storage clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	int clientSocketFD = accept(_serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressLength);

	if (clientSocketFD < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK){}
		else
			ErrorLogger(FAILED_SOCKET_ACCEPT, __FILE__, __LINE__, true);
	}
	struct kevent evSet;
	EV_SET(&evSet, clientSocketFD, EVFILT_READ, EV_ADD, 0, 0, NULL);

	if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1){
		close(clientSocketFD);
		ErrorLogger(FAILED_SOCKET_KQUEUE_KEVENT, __FILE__, __LINE__);
	}

	char hostname[NI_MAXHOST];
	if (_serverSocketFamily == AF_INET){
		if (inet_ntop(AF_INET, &(((struct sockaddr_in *)&clientAddress)->sin_addr), hostname, sizeof(hostname)) == NULL)
			ErrorLogger(FAILED_SOCKET_GETADDRINFO, __FILE__, __LINE__, true);
	}
	else
	{
		if (inet_ntop(AF_INET6, &(((struct sockaddr_in *)&clientAddress)->sin_addr), hostname, sizeof(hostname)) == NULL)
			ErrorLogger(FAILED_SOCKET_GETADDRINFO, __FILE__, __LINE__, true);
		}

	Client* client = NULL;
	if (clientAddress.ss_family == AF_INET)
		client = new Client(clientSocketFD, ntohs(((struct sockaddr_in*)&clientAddress)->sin_port), hostname, _serverName);
	else if (clientAddress.ss_family == AF_INET6)
		client = new Client(clientSocketFD, ntohs(((struct sockaddr_in6*)&clientAddress)->sin6_port), hostname, _serverName);

	_clients.insert(std::make_pair(clientSocketFD, client));

	std::ostringstream messageStream;
	messageStream << "New Client: has connected.";
	client->sendMessage("Connected to Server");
	client->sendMessage("Enter the server password using /PASS");

	log(messageStream.str());

	return clientSocketFD;
}
// Listen for existing connections and accept new connections with `epoll` or `kqueue`.
// New Client connect and check MAX CLIENTS
// Bot is created and listening.
void Server::serverRun()
{
	socketStart();
	socketInit();
	socketBind();
	socketListen();
	signal(SIGINT, signalHandler);
	try
	{
		_bot = new Bot("localhost", _serverSocketPort, _serverPass);
		struct kevent evSet;
		EV_SET(&evSet, _bot->getSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);

		if (kevent(kq, &evSet, 1, NULL, 0, NULL) == -1)
			perror("kevent: Bot socket");
	}
	catch (const std::exception &e)
	{
		delete _bot;
		_bot = NULL;
		write(STDOUT_FILENO, e.what(), strlen(e.what()));
	}
	while (true)
	{
		struct kevent evList[MAX_CLIENTS];
		int n = kevent(kq, NULL, 0, evList, MAX_CLIENTS, NULL);
		for (int i = 0; i < n; i++) {
			if (static_cast<int>(evList[i].ident) == _serverSocketFD) {
				int clientFD = socketAccept();
				if (clientFD != -1) {
				}
			} else {
				if (evList[i].filter == EVFILT_READ) {
					handleClient(evList[i].ident);
				}
			}
		}
	}
}
