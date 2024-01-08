#include "../include/Server.hpp"

Server* Server::ins = NULL;

// Singleton tasarım deseni kullanılarak tek bir Server nesnesinin oluşturulması sağlanır.
// Bu fonksiyon, Server sınıfının örneklemesini döndürür veya oluşturur.
Server::Server(int serverSocketFamily, int serverSocketProtocol, int serverSocketPort, string serverName)
	: _serverSocketFD(-1),
	_serverSocketFamily(serverSocketFamily),
	_serverSocketProtocol(serverSocketProtocol),
	_serverSocketPort(serverSocketPort),
	_serverName(serverName),
	_serverPass(""),
	_bot(NULL)
{
	// Ctrl+C sinyali alındığında tetiklenecek sinyal işleyici atanır.
	signal(SIGINT, signalHandler);
	Server::setInstance(this);	// Singleton

	// epoll(linux) veya kqueue(BSD) için istemci isteklerini dinlemek için bir dosya tanımlayıcısı oluşturulur.
	memset(&serverAddress, 0, sizeof(serverAddress));

	epollFd = epoll_create1(0);
	if (epollFd == -1) {
	}

}

// Server nesnesinin yıkıcısı.
// Ayrıca, Server sınıfının tek bir örneğini tutan işaretçi silinir ve bellek temizlenir.
Server::~Server()
{
	delete Server::ins;
	Server::ins = NULL;

	// Tüm istemciler ve kanallar bellekten temizlenir.
	for (map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
	_clients.clear();
	for (map<string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete it->second;
	}
	_channels.clear();

	// Server soketi kapatılır.
	if (_serverSocketFD != -1)
		close(_serverSocketFD);

	if (epollFd != -1)
	{
		close(epollFd);
	}
	if (_bot != NULL){
		delete _bot;
	}

}
// Server soketini oluşturur.
// Soketin dosya tanımlayıcısını döndürür.
// NONBLOCK -> Soketi engellemeyen (non-blocking) modda ayarlar, yani soketi bloklamadan okuma ve yazma işlemlerini gerçekleştirebiliriz.
// reuse -> Soketi yeniden kullanılabilir (reusable) hale getirir.
// reuse -> Bu, sunucunun hemen kapatıldıktan sonra yeniden başlatılabilmesi için gereklidir.
void Server::socketStart()
{
	// Soket oluşturulur.
	_serverSocketFD = socket(_serverSocketFamily, _serverSocketProtocol, 0);

	if (_serverSocketFD == -1)
		ErrorLogger(FAILED_SOCKET, __FILE__, __LINE__);

	// Soketi non-blocking moda ayarlar.
	if (fcntl(_serverSocketFD, F_SETFL, O_NONBLOCK) == -1)
	{
		close(_serverSocketFD);
		ErrorLogger(FAILED_SOCKET_NONBLOCKING, __FILE__, __LINE__);
	}

	// Soketin tekrar kullanılabilir olmasını sağlar.
	int reuse = 1;
	if (setsockopt(_serverSocketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
	{
		close(_serverSocketFD);
		ErrorLogger(FAILED_SOCKET_OPTIONS, __FILE__, __LINE__);
	}
}

// Soketin gerekli bağlantı argümanlarını tanımlar.
void Server::socketInit()
{
	switch (_serverSocketFamily)
	{
		case AF_INET: // IPv4
			serverAddress.sin_addr.s_addr = INADDR_ANY;
			serverAddress.sin_family = _serverSocketFamily;
			serverAddress.sin_port = htons(_serverSocketPort);
			break;
			
		default:
			close(_serverSocketFD);
			ErrorLogger(FAILED_SOCKET_DOMAIN, __FILE__, __LINE__);
	}
}
// AF_INET Bağlantı noktasını belirtmek için server soketinin adresini bağlar.
void Server::socketBind()
{

			if (::bind(_serverSocketFD, reinterpret_cast<struct sockaddr*>(&serverAddress), sizeof(serverAddress)) == -1)
			{
				close(_serverSocketFD);
				ErrorLogger(FAILED_SOCKET_BIND, __FILE__, __LINE__);
			}

}

// listen -> Sunucu soketi üzerinde dinleniyor
// Kısacası, sunucu istekleri dinlemeye başlar.
// epoll veya kqueue ile dinleniyor, çoklu istemci desteği için.
// EPOLLIN -> Okunabilir veri mevcut
// EVFILT_READ -> Okunabilir veri mevcut
void Server::socketListen()
{
	if ( listen(_serverSocketFD, BACKLOG_SIZE) == -1 )
	{
		close(_serverSocketFD);
		ErrorLogger(FAILED_SOCKET_LISTEN, __FILE__, __LINE__);
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = _serverSocketFD;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _serverSocketFD, &ev) == -1)
	{
		perror("epoll_ctl: server socket");
	}
}

// Yeni bir istemci bağlantısını kabul eder.
// Yeni bir istemci bağlantısını alır ve yeni soketin dosya tanımlayıcısını döndürür.
// EPOLLET -> Edge Triggered: Olay, soketin durumu değiştiğinde yalnızca tetiklenir.
int Server::socketAccept()
{
	struct sockaddr_storage clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	int clientSocketFD = accept(_serverSocketFD, (struct sockaddr *)&clientAddress, &clientAddressLength);

	if (clientSocketFD < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
		}
		else
		{
			ErrorLogger(FAILED_SOCKET_ACCEPT, __FILE__, __LINE__, true);
		}
	}

	// Soketi non-blocking moda ayarlar.
	if (fcntl(clientSocketFD, F_SETFL, O_NONBLOCK) == -1)
	{
		close(clientSocketFD);
		ErrorLogger(FAILED_SOCKET_NONBLOCKING, __FILE__, __LINE__);
	}

	// Soketin tekrar kullanılabilir olmasını sağlar.
	int reuse = 1;
	if (setsockopt(clientSocketFD, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
	{
		close(clientSocketFD);
		ErrorLogger(FAILED_SOCKET_OPTIONS, __FILE__, __LINE__);
	}


	struct epoll_event event;
	event.data.fd = clientSocketFD;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocketFD, &event) == -1)
	{
		close(clientSocketFD);
		ErrorLogger(FAILED_SOCKET_EPOLL_CTL, __FILE__, __LINE__);
	}

	char hostname[NI_MAXHOST];
	if (inet_ntop(AF_INET, &(((struct sockaddr_in *)&clientAddress)->sin_addr), hostname, sizeof(hostname)) == NULL)
	{
		ErrorLogger(FAILED_SOCKET_GETADDRINFO, __FILE__, __LINE__, true);
	}

	Client* client = NULL;

	client = new Client(clientSocketFD, ntohs(((struct sockaddr_in*)&clientAddress)->sin_port), hostname, _serverName);

	_clients.insert(std::make_pair(clientSocketFD, client));

	std::ostringstream messageStream;
	messageStream << "New Client: has connected.";
	client->sendMessage("Connected to Server");
	client->sendMessage("Enter the server password using /PASS");

	log(messageStream.str());

	return clientSocketFD;
}
// Mevcut bağlantıları dinler ve yeni bağlantıları `epoll` veya `kqueue` ile kabul eder.
// Yeni istemci bağlanır ve maksimum istemci sayısını kontrol eder.
// Bot oluşturulur ve dinlenir.
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

		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = _bot->getSocket();

		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _bot->getSocket(), &ev) == -1)
		{
			perror("epoll_ctl: Bot socket");
		}

	}
	catch (const std::exception &e)
	{
		// Bot oluşturulamazsa, hata mesajı yazdırılır.
		delete _bot;
		_bot = NULL;
		write(STDOUT_FILENO, e.what(), strlen(e.what()));
	}

	// Ana döngü, kqueue ile olayları dinler.
	while (true)
	{
		struct epoll_event events[MAX_CLIENTS];
		int n = epoll_wait(epollFd, events, MAX_CLIENTS, -1);
		for (int i = 0; i < n; i++) {
			if (events[i].data.fd == _serverSocketFD) {
				int clientFD = socketAccept();
				if (clientFD != -1) {
					// Maksimum istemci sayısını kontrol et.
					// Eğer maksimum sayıya ulaşılmışsa bağlantıyı kapat.
				}
			} else {
				if (events[i].events & EPOLLIN) {
					handleClient(events[i].data.fd);
				}
			}
		}
	}
}
