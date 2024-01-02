#include "../../include/Commands.hpp"

void Quit::quit( Client* client, vector<string> commandParts, Server* srv)
{
	string reason = commandParts.empty() ? "Leaving from Server" : commandParts.at(0);

	if ( reason.at(0) == ':' )
		reason.substr(1);

	client->sendMessage(RPL_QUIT(client->getPrefix(), reason));
	srv->removeClientFromAllChannels( client );
	srv->clientDisconnect( client->getClientSocketFD() );
}
