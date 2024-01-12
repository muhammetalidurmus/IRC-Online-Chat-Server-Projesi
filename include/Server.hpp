#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <map>
#include <csignal>
#include <unistd.h>
#include "Bot.hpp"
#include "Utils.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Commands.hpp"
#include <sstream>

class Client;
class Channel;
class Bot;

using namespace std;

class Server
{
	private:
		int _serverSocketFD;
		const int _serverSocketFamily; // IPV4 V6 etc..
		const int _serverSocketProtocol; // TCP/IP - UDP
		const int _serverSocketPort; // 1234 2345 etc..
		string _serverName;
		string	_serverPass;


		static Server *ins;

		struct sockaddr_in serverAddress;

		fd_set read_set;

		map<int, Client> clientBuffers;
		map<int, Client*> _clients;
		map<string, Channel*> _channels;
		Bot* _bot;

		void socketStart();
		void socketInit();
		void socketBind();
		void socketListen();
		int socketAccept();

	public:
		Server();
		Server ( int serverSocketFamily, int serverSocketProtocol, int serverSocketPort, string serverName );
		~Server();

		map<int, Client*> getAllClients() {
		return _clients;
		}
		void serverRun();
		void shutdownSrv();

		//SIGNAL
		static void signalHandler(int signum);
		static void signalHandlerServer(int signum);
		void handleClient(int clientSocketFD);
		static Server* getInstance() {return ins;}
		static void setInstance(Server* server){ins = server;}
		Client* getClient( string& nickName );
		void removeClientFromAllChannels( Client* client );
		void clientDisconnect(int clientSocketFD);
		void setSrvPass(const string& pass);
		string getSrvPass() const{return _serverPass;}
		void addChannel( Channel* channel );
		Channel* getChannel( string& channelName );
		bool channelExists( const string& channelName );
		bool verifySrvPass(const string& pass);
		void removeChannel(const string& channel );
		void processPartialCommands(int clientSocketFD);
		Bot* getBot() { return _bot; }

};

#endif
