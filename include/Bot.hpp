#pragma once

#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <string>
#include <unistd.h>

using namespace std;

class Bot {
	private:
		int		sock;
		string	serv;
		int		port;
		string	botPass;
		string	nick;
		string	user;
		string	realname;

	public:
		Bot(const string &serv, int port, const string &pass);
		~Bot();

		string getBotnick() const { return nick; }
		void connectServ();
		void sendMsg(const string &channel, const string &msg);
		void sendRegMsg(const string &msg);
		void listen();
		void processMessage(const string &msg);

		int getSocket() const { return sock; }

		void WelcomeMsg(const string &userNick) {
			sendMsg(userNick, "Welcome! We've been waiting for you!");
		}
};

