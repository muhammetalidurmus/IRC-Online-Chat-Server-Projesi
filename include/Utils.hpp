#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <exception>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <ctime>


using namespace std;


#define GET_CURRENT_TIME time(0)
#define FORMAT_TIME(t, buffer) strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", localtime(&t))

/* SERVER FAILED/ERROR MESSAGES */
#define FAILED_SOCKET "Failed to create socket"
#define FAILED_SOCKET_OPTIONS "Failed to set socket options"
#define FAILED_SOCKET_NONBLOCKING "Failed to set socket to non-blocking"
#define FAILED_SOCKET_BIND "Failed to bind socket"
#define FAILED_SOCKET_LISTEN "Failed to listen socket"
#define FAILED_SOCKET_ACCEPT "Failed to accept socket"
#define FAILED_SOCKET_CONNECT "Failed to connect socket"
#define FAILED_SOCKET_SEND "Failed to send socket"
#define FAILED_SOCKET_RECV "Failed to recv socket"
#define FAILED_SOCKET_CLOSE "Failed to close socket"
#define FAILED_SOCKET_GETADDRINFO "Failed to getaddrinfo socket"
#define FAILED_SOCKET_DOMAIN "Not supported domain"
#define FAILED_SOCKET_RECEIVE "Failed to receive socket"

/* SERVER DEFINE */
#define BACKLOG_SIZE 100
#define MAX_CLIENTS 100

/* CLIENT FAILED/ERROR MESSAGES */
#define ERROR_SOCKET_SEND "Error while sending a message to a client!"
	/* REPLY */
#define WELCOME_MESSAGE(source3, source) "Welcome to the " + source3 + " " + source + "!"
#define RPL_NAMREPLY(source, channel, nickList) ":" + source + " 353 " + source + " = " + channel + " :" + nickList
/* COMMAND RESPONSE */
#define RPL_QUIT(source, message) ":" + source + " QUIT :Quit: " + message
#define ERR_ALREADYREGISTERED(source) "462 " + source + " :You may not register"
#define ERR_NEEDMOREPARAMS(source, command) "461 " + source + " " + command + " :Not enough parameters"
#define ERR_NOSUCHCHANNEL(source, channel) "403 " + source + " " + channel + " :No such channel"
#define ERR_USERONCHANNEL(source, channel) "443 " + source + " " + channel + " :is already on channel"
#define ERR_BADCHANNELKEY(source, channel) "475 " + source + " " + channel + " :Cannot join channel (+k)"
#define ERR_INVITEONLYCHAN(source, channel) "473 " + source + " " + channel + " :Cannot join channel (+i)"
#define ERR_CHANNELISFULL(source, channel) "471 " + source + " " + channel + " :Cannot join channel (+l)"
#define RPL_JOIN(source, channel) ":" + source + " JOIN :" + channel
#define RPL_ENDOFNAMES(source, channel) "366 " + source + " " + channel + " :End of /NAMES list."
#define ERR_CHANOPRIVSNEEDED(source, channel) "482 " + source + " " + channel + " :You're not channel operator"
#define ERR_FLAGNOSUCH(source, flag) "501 " + source + " " + flag + " :Unknown MODE flag"
#define ERR_USERSDONTMATCH(source) "502 " + source + " :Cant change mode for other users"
#define ERR_NOTONCHANNEL(source, channel) "442 " + source + " " + channel + " :You're not on that channel"
#define ERR_NOSUCHNICK(source, nick) "401 " + source + " " + nick + " :No such nick"
#define ERR_USERNOTINCHANNEL(source, nick, channel) "441 " + source + " " + nick + " " + channel + " :They aren't on that channel"
#define ERR_NONICKNAMEGIVEN(source) "431 " + source + " :No nickname given"
#define ERR_ERRONEUSNICKNAME(source, nick) "432 " + source + " " + nick + " :Erroneous nickname"
#define ERR_NICKNAMEINUSE(source, nick) "433 " + source + " " + nick + " :Nickname is already in use"
#define ERR_NICKCOLLISION(source, nick) "436 " + source + " " + nick + " :Nickname collision KILL"
#define ERR_INVALIDCAPCMD(source) "410 " + source + " :Invalid CAP subcommand"
#define ERR_NOTEXTTOSEND(source) "412 " + source + " :No text to send"
#define ERR_NOCANNOTSENDTOCHAN(source, channel) "404 " + source + " " + channel + " :Cannot send to channel"
#define ERR_NOKICKCHANNELOWNER(source, channel) source + " " + channel + " :Cannot kick to channel owner!"

static inline void ErrorLogger( string messageInfo, const char* fileInfo, int lineInfo, bool isFatal = false )
{
	ofstream errorLog;

	errorLog.open("error.log", ios::app);
	errorLog << "Error Time: " << __TIME__ << endl;
	errorLog << "Error in file: " << fileInfo << ":" << lineInfo << endl;
	errorLog << "Error info: " << string(strerror(errno)) << endl;
	errorLog << "----------------------------------------" << endl;
	errorLog.close();

	if (isFatal) {
		throw runtime_error(messageInfo);
	}
	write(STDERR_FILENO, messageInfo.c_str(), messageInfo.length());
}

static inline void log(const string& message) {
	char buffer[100];
	time_t currentTime = time(NULL);
	ofstream logFile;


	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
	string timeStr(buffer);

	logFile.open("log.log", std::ios::app);
	logFile << timeStr << " " << message << std::endl;
	logFile.close();


	ostringstream oss;
	oss << "\033[0;34m[" << timeStr << "]\033[0m " << message << std::endl;

	string outputStr = oss.str();
	write(STDOUT_FILENO, outputStr.c_str(), outputStr.size());
}

#endif
