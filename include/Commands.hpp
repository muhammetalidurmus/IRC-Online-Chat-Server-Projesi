#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include "Client.hpp"
#include "Server.hpp"
#include "Channel.hpp"
#include "Bot.hpp"

class Client;
class Server;

class CommandParser
{
	private:
		CommandParser();
		~CommandParser();
	public:
		static void commandParser( const char *command, Client* client, Server* srv );
		static void handleCommand( Client* client, vector<string> commandParts, Server* srv );
};

class Quit
{
	private:
		Quit();
		~Quit();
	public:
		static void quit( Client* client, vector<string> commandParts, Server* srv );
};

class User
{
	private:
		User();
		~User();
	public:
		static void user( Client* client, vector<string> commandParts, Server* srv );
};

class PrivMsg
{
	private:
		PrivMsg();
		~PrivMsg();
	public:
		static void privMsg( Client* client, vector<string> commandParts, Server* srv );
};

class Join
{
	private:
		Join();
		~Join();
	public:
		static void join( Client* client, vector<string> commandParts, Server* srv );
		static void joinChannel( Client* client, string channelName, vector<string> commandParts, Server* srv );
};

class Part
{
	private:
		Part();
		~Part();
	public:
		static void part( Client* client, vector<string> commandParts, Server* srv );
		static void partChannel( Client* client, string channelName, Server* srv );
};

class Kick
{
	private:
		Kick();
		~Kick();
	public:
		static void kick( Client* client, vector<string> commandParts, Server* srv );
};

class Mode
{
	private:
		Mode();
		~Mode();
	public:
		static void mode( Client* client, vector<string> commandParts, Server* srv );
};

class Notice
{
	private:
		Notice();
		~Notice();
	public:
		static void notice( Client* client, vector<string> commandParts, Server* srv );
};

class Pass
{
	private:
		Pass();
		~Pass();
	public:
		static void pass( Client* client, vector<string> commandParts, Server* srv );
};

class Nick
{
	private:
		Nick();
		~Nick();
	public:
		static void nick( Client* client, vector<string> commandParts, Server* srv );
};

class Cap
{
	private:
		Cap();
		~Cap();
	public:
		static void cap( Client* client, vector<string> commandParts);
};

void sendChannelMessage( Client* client, string channelName, string message, Server* srv );
void sendPrivateMessage( Client* client, string target, string message, Server* srv );
string mergeString(const vector<string>& parts, const string& delimiter);
string atrim(const std::string& str);
string strim(const string& str);

#endif
